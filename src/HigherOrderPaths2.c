#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>

typedef enum categories
{
	health,
	magazine,
	sport
} categories;

int documentcount=0;
char * categori_names [3];

typedef struct wnode
{
	char * word; //kelime
	int tf;//tüm dökümanlarda kaç kere geçtiği
	int df;//kaç dökümanda geçtiği
	double tfidf; // hesap
	int isInCategory[3]; //kelimenin katetoride geçip geçmediği 0 ya da 1
	int documentno; // herdökümanda df değerini bir kere sayması için kullanılan kontrol
	struct wnode * left;
	struct wnode * right;
}wnode;

//en sol node, top, root
wnode * top=NULL;

//karakter olup olmadığının testi
//rakamlar özel karakterler alınmaz
int isCharacter(char c)
{
	int isChar=0;
	if((c>='a' && c<='z') || (c>='A' && c<='Z'))
	{
		isChar=1;
	}
	return isChar;

}

//iki kelimenin karşılaştırılması
int isEqual(char * word1, char * word2)
{
	int w1size=strlen(word1);
	int w2size=strlen(word2);
	if(w1size!=w2size)
		return 0;

	int i=0;
	for(i=0; i<w1size; i++)
	{
		if(word1[i]!=word2[i])//iki karakter eşit değilse 0 dön kelimeler eşit değil
			return 0;
	}

	return 1;


}


int category=-1; //aynı katogoride olup olmadığı

int readFiles(char * categori_name)
{
	DIR * FD; //c veri yapısı


	if (NULL == (FD = opendir(categori_name)))//klasorü aç
	{
		fprintf(stderr, "Error : Failed to categori input directory\n");
		return 1;
	}

	//hangi kateforideyiz 0-healty 1-magazine 2-sport
	int k=0;
	for(k=0; k<3; k++)
	{
		if(isEqual(categori_names[k],categori_name)==1) // şuan hangi kategorideyiz
		{
			category=k;//okunan kelimelerin kategorisini işaretmelemek için
			break;

		}

	}


	struct dirent* fileProperties;
	int extentLen=4;
	char extent[]={'.','t','x','t'};
	while ((fileProperties = readdir(FD)))
	{


		if(fileProperties->d_namlen>4)
		{
			//sonu .txt ile biten dosyalari aliyoruz
			int startLoc=fileProperties->d_namlen-4;//tem.txt 7 uzunluğunda -4 yaparsam 3. indeksten karşılaştırma başlar
			int isEndWithTXT=1;
			int i=0;
			for(i=0; i<4; i++)
			{
				if(extent[i]!=fileProperties->d_name[startLoc+i])
				{
					isEndWithTXT=0;
					break;
				}
			}
			//*************************

			// eger sonu txt ile bitiyorsa okuma islemini gerceklestiriyoruz
			if(isEndWithTXT==1)
			{
				//printf("%s %d\n",fileProperties->d_name,fileProperties->d_namlen);

				//open file with path
				int categorinamelen=strlen(categori_name);
				char * path= (char *) calloc((categorinamelen+fileProperties->d_namlen+1),sizeof(char));//kategori+/+filename
				strcpy(path,categori_name); // path=categori
				path[categorinamelen]='\\'; ///path=categori/
				strcat(path,fileProperties->d_name); // path= categori/file.txt

				FILE* fin = fopen(path, "r");//txt dosyayı aç

				if (fin == NULL)//açılmazsa hataver
				{
					fprintf(stderr, "Error : Failed to file input directory\n");
					return 1;
				}
				//********************************************

				documentcount++; //dokuman sayısını artır

				char * aword=(char *)calloc(30, sizeof(char));
				int awordindex=0;
				int wordstart=0;
				char c;
				while ((c = getc(fin)) != EOF)//dosyadan krakter karakter oku
				{
					/*if(c=='Ã')
			    		continue;*/
					if(isCharacter(c)==1)// gelen karakterse
					{

						aword[awordindex]=c;//aword kelimesine ekle
						awordindex++;//sonraki index

					}
					else if(c==' ' || c=='\n')
					{
						if(awordindex==0)
							continue;

						char nword[awordindex+1]; //aword 30 karakter nword tam uzunlukta. dizi sıfırdan başladığı için +1
						int i=0;
						for(i=0; i<=awordindex; i++)//kopyalama
						{
							nword[i]=aword[i];
						}

						wnode * wtraverse;
						int isfind=0;
						if(top==NULL)
						{
							top=(wnode*) malloc(sizeof( wnode));
							top->left=NULL;
							top->right=NULL;
							top->word=(char *) calloc(strlen(nword),sizeof(char));
							top->tf=1;
							strcpy(top->word,nword);
							top->documentno=documentcount;
							top->df=1;
							top->tfidf=0;
							int i=0;
							for(i=0; i<3; i++)
							{
								top->isInCategory[i]=0;
							}
							top->isInCategory[category]=1;
							wtraverse=top;
						}
						else
						{
							wtraverse=top;
							while(wtraverse->right!=NULL)
							{
								if(isEqual(wtraverse->word,nword)==1)//daha önceden listeye eklimi
								{
									wtraverse->tf++; //ekliyse sadece tf artır
									//eger listede varsa
									// o an okunan kategoride isaretlenmemişse
									if(wtraverse->isInCategory[category]==0)
									{
										wtraverse->isInCategory[category]=1;
									}

									if(wtraverse->documentno!=documentcount)//bir file için bir kelime bir kere df sayılır
									{
										wtraverse->df++;
										wtraverse->documentno=documentcount;
									}

									isfind=1;//kelime listede varsa bulundu
									break;
								}
								wtraverse=wtraverse->right;
							}
							if(isfind==0)//sona ekle
							{
								wtraverse->right=(wnode*) malloc(sizeof( wnode));
								wtraverse->right->right=NULL;
								wtraverse->right->left=wtraverse;
								wtraverse->right->word=(char *) calloc(awordindex,sizeof(char));
								strcpy(wtraverse->right->word,nword);
								wtraverse->right->tf=1;
								wtraverse->right->df=1;
								wtraverse->right->documentno=documentcount;
								wtraverse->right->tfidf=0;

								int i=0;
								for(i=0; i<3; i++)
								{
									wtraverse->right->isInCategory[i]=0;
								}
								wtraverse->right->isInCategory[category]=1;

							}
						}


						aword= (char *)calloc(30, sizeof(char));
						awordindex=0;
						//printf("%s\n",nword);

					}


				}
				fclose(fin);

			}


		}


	}



}

void printAll()
{
	if(top==NULL)
	{
		printf("list is empty\n");
		return;
	}

	wnode * wtraverse= top;
	while(wtraverse!=NULL)
	{
		printf("%s | TF: %d | IDF: %d | TF/IDF: %.2f | H: %d M: %d S: %d\n",wtraverse->word,wtraverse->tf,wtraverse->df,wtraverse->tfidf,wtraverse->isInCategory[0],wtraverse->isInCategory[1],wtraverse->isInCategory[2]);
		fflush( stdout ); //ekrana yazdırmadığında

		wtraverse= wtraverse->right;
	}


}
void calculateTFIDF()
{
	if(top==NULL || documentcount==0)
	{
		printf("list is empty\n");
		return;
	}

	wnode * wtraverse= top;
	while(wtraverse!=NULL)
	{
		wtraverse->tfidf= wtraverse->tf*(log(((double)documentcount)/((double)wtraverse->df)));//verilen formul

		wtraverse= wtraverse->right;
	}
	printf("\n");

}
void print10IDFTF()
{
	printf("IDF Frequency Order\n");
	wnode * nodes10[10];
	int i=0;
	for(i=0; i<10; i++)
	{
		nodes10[i]=NULL;


	}

	wnode * wtraverse= top;
	while(wtraverse!=NULL)
	{
		int i=0;
		for(i=0; i<10; i++)
		{
			if(nodes10[i]==NULL)
			{
				nodes10[i]=wtraverse;
				break;
			}
			else if(nodes10[i]->tfidf < wtraverse->tfidf)//tf yerine tfidf karşılaştırması
			{

				int z=9;
				for(z=9; z>i; z--)
				{
					wnode * temp2=nodes10[z-1];
					nodes10[z]=temp2;
				}
				nodes10[i]=wtraverse;
				break;
			}
		}

		wtraverse= wtraverse->right;
	}

	int l=0;
	for(l=0; l<10; l++)
	{
		//printf("dsad\n");
		printf("%s:%.2f\n",nodes10[l]->word,nodes10[l]->tfidf);
		fflush( stdout );
	}
	printf("\n");

}
void print10TF()
{
	printf("Term Frequency Order\n");
	wnode * nodes10[10];
	int i=0;
	for(i=0; i<10; i++)
	{
		nodes10[i]=NULL;


	}

	wnode * wtraverse= top;
	while(wtraverse!=NULL)
	{
		int i=0;
		for(i=0; i<10; i++)
		{
			if(nodes10[i]==NULL)
			{
				nodes10[i]=wtraverse;
				break;
			}
			else if(nodes10[i]->tf < wtraverse->tf)//eğer yeni gelen büyükse i ninci ile değiştir
			{

				int z=9;
				for(z=9; z>i; z--)  // sıra yukrı kaydırma kaydırma
				{
					wnode * temp2=nodes10[z-1];
					nodes10[z]=temp2;
				}
				nodes10[i]=wtraverse;
				break;
			}
		}

		wtraverse= wtraverse->right;
	}

	int l=0;
	for(l=0; l<10; l++)
	{
		//printf("dsad\n");
		printf("%s:%d\n",nodes10[l]->word,nodes10[l]->tf);
		fflush( stdout );
	}


}

void print1stCoOccurence()
{
	printf("1.ST-order term co-occurence ");
	wnode * wtraverse= top;
	while(wtraverse!=NULL)//kelimeyi al
	{
		wnode * wtraverse2= wtraverse->right;
		while(wtraverse2!=NULL)//tüm kelimlerle karşılaştır
		{
			if((wtraverse->isInCategory[0]==1 && 1==wtraverse2->isInCategory[0])||(wtraverse->isInCategory[1]==1 && 1==wtraverse2->isInCategory[1])||(wtraverse->isInCategory[2]==1 && 1==wtraverse2->isInCategory[2]))
			{
				printf("{%s,%s},",wtraverse->word,wtraverse2->word);
				fflush( stdout );
			}
			wtraverse2=wtraverse2->right;
		}
		wtraverse=wtraverse->right;
	}
	printf("\n");
}


// t1  100
// t2  110
// t3  010

void print2stCoOccurence()
{
	printf("2.ST-order term co-occurence ");
	wnode * wtraverse1= top;

	while(wtraverse1!=NULL)
	{
		wnode * wtraverse2= wtraverse1;
		while(wtraverse2!=NULL)
		{
			if(!((wtraverse1->isInCategory[0]==1 && 1==wtraverse2->isInCategory[0])||(wtraverse1->isInCategory[1]==1 && 1==wtraverse2->isInCategory[1])||(wtraverse1->isInCategory[2]==1 && 1==wtraverse2->isInCategory[2])))
			{
				wnode * wtraverse3= top;
				while(wtraverse3!=NULL)
				{
					if(isEqual(wtraverse1->word,wtraverse3->word) !=1 && isEqual(wtraverse2->word,wtraverse3->word) !=1)// ele aldığım iki kelime dışında bir kelime
					{
						if((wtraverse3->isInCategory[0]+wtraverse3->isInCategory[1]+wtraverse3->isInCategory[2])>1)//kelimenin kategori toplamı 1 den büyük olmalıki iki kategori bağlauyabilsin
						{
							int isConnected=0;

							int i=0;
							for(i=0; i<3&&isConnected==0; i++)//karegori arası dolaşmak için
							{
								if(wtraverse1->isInCategory[i]==1 && 1==wtraverse3->isInCategory[i])
								{
									int m=0;
									for(m=0; m<3; m++)
									{
										if(wtraverse2->isInCategory[m]==1 && 1==wtraverse3->isInCategory[m])
										{
											isConnected=1;
											break;
										}
									}

								}
							}
							if(isConnected==1)
							{
								/*printf("{%d,%d,%d} ",wtraverse1->isInCategory[0],wtraverse1->isInCategory[1],wtraverse1->isInCategory[2]);
								printf("{%d,%d,%d} ",wtraverse2->isInCategory[0],wtraverse2->isInCategory[1],wtraverse2->isInCategory[2]);
								printf("{%d,%d,%d} ",wtraverse3->isInCategory[0],wtraverse3->isInCategory[1],wtraverse3->isInCategory[2]);
								printf("{%s,%s,%s},\n",wtraverse1->word,wtraverse2->word,wtraverse3->word);
								*/

								printf("{%s,%s},",wtraverse1->word,wtraverse2->word);
								fflush( stdout );
								break;
							}

						}
					}
					wtraverse3=wtraverse3->right;
				}
			}
			wtraverse2=wtraverse2->right;
		}
		wtraverse1=wtraverse1->right;
	}
	printf("\n");
}

void print3stCoOccurence()
{
	printf("3.ST-order term co-occurence ");
	wnode * wtraverse1= top;

	while(wtraverse1!=NULL)
	{
		wnode * wtraverse2= wtraverse1;
		while(wtraverse2!=NULL)
		{
			int isConnected=0;
			if(!((wtraverse1->isInCategory[0]==1 && 1==wtraverse2->isInCategory[0])||(wtraverse1->isInCategory[1]==1 && 1==wtraverse2->isInCategory[1])||(wtraverse1->isInCategory[2]==1 && 1==wtraverse2->isInCategory[2])))
			{
				wnode * wtraverse3= top;
				while(wtraverse3!=NULL)
				{
					if(isEqual(wtraverse1->word,wtraverse3->word) !=1 && isEqual(wtraverse1->word,wtraverse3->word) !=1)
					{
						if((wtraverse3->isInCategory[0]+wtraverse3->isInCategory[1]+wtraverse3->isInCategory[2])>1)
						{


							int i=0;
							for(i=0; i<3&&isConnected==0; i++)
							{
								if(wtraverse1->isInCategory[i]==1 && 1==wtraverse3->isInCategory[i])
								{
									int m=0;
									for(m=0; m<3; m++)
									{
										if(wtraverse2->isInCategory[m]==1 && 1==wtraverse3->isInCategory[m])
										{
											isConnected=1;
											break;
										}
									}

								}
							}


						}
					}
					wtraverse3=wtraverse3->right;
				}
				if(isConnected!=1)
				{
					/*printf("{%d,%d,%d} ",wtraverse1->isInCategory[0],wtraverse1->isInCategory[1],wtraverse1->isInCategory[2]);
					printf("{%d,%d,%d} ",wtraverse2->isInCategory[0],wtraverse2->isInCategory[1],wtraverse2->isInCategory[2]);
					printf("{%s,%s,\n",wtraverse1->word,wtraverse2->word);
					*/
					printf("{%s,%s},",wtraverse1->word,wtraverse2->word);
					fflush( stdout );

				}


			}


			wtraverse2=wtraverse2->right;
		}
		wtraverse1=wtraverse1->right;
	}
	printf("\n");
}

int main(int argc, char *argv[]) {
	//readFiles(".");
	categori_names[0]= (char *) calloc((6),sizeof(char));
	strcpy(categori_names[0],"health");

	categori_names[1]= (char *) calloc((8),sizeof(char));
	strcpy(categori_names[1],"magazine");

	categori_names[2]= (char *) calloc((5),sizeof(char));
	strcpy(categori_names[2],"sport");

	readFiles(categori_names[0]);
	readFiles(categori_names[1]);
	readFiles(categori_names[2]);

	//printAll();
	calculateTFIDF();
	//printAll();
	printf("\n");
	print10TF();
	printf("\n");
	print10IDFTF();
	printf("\n");

	print1stCoOccurence();
	printf("\n");
	print2stCoOccurence();
	printf("\n");
	print3stCoOccurence();
	printf("\n");
	system("pause");
	return 0;
}
