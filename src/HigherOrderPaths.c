//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <sys/types.h>
//#include <dirent.h>
//#include <unistd.h>
//#include <errno.h>
//#include <wchar.h>
//#include <locale.h>
//
//typedef struct wnode
//{
//	char * word;
//	int tf;
//	int idf;
//	double tfidf;
//	struct wnode * left;
//	struct wnode * right;
//}wnode;
//
//typedef struct afile
//{
//	char * name;
//	int wordcount;
//	char * categori_type;
//	wnode * top;
//
//}afile;
//
//typedef struct fnode
//{
//
//	afile* file;
//	struct fnode * left;
//	struct fnode * right;
//}fnode;
//
//
//fnode * top=NULL;
//int isCharacter(char c)
//{
//	int isChar=0;
//	if((c>='a' && c<='z') || (c>='A' && c<='Z'))
//	{
//		isChar=1;
//	}
//	return isChar;
//
//}
//
//int isEqual(char * word1, char * word2)
//{
//	int w1size=strlen(word1);
//	int w2size=strlen(word2);
//	if(w1size!=w2size)
//		return 0;
//	int i=0;
//	for(i=0; i<w1size; i++)
//	{
//		int j=0;
//		for(j=0; j<w1size; j++)
//		{
//			if(word1[i]!=word2[i])
//				return 0;
//		}
//	}
//
//	return 1;
//
//
//}
//
//
//int readFiles(char * categori_name)
//{
//	DIR * FD;
//	struct dirent* fileProperties;
//
//	if (NULL == (FD = opendir(categori_name)))
//	{
//		fprintf(stderr, "Error : Failed to categori input directory\n");
//		return 1;
//	}
//
//	int extentLen=4;
//	char extent[]={'.','t','x','t'};
//	while ((fileProperties = readdir(FD)))
//	{
//
//
//		if(fileProperties->d_namlen>4)
//		{
//			//sonu .txt ile biten dosyalari aliyoruz
//			int startLoc=fileProperties->d_namlen-4;
//			int isEndWithTXT=1;
//			int i=0;
//			for(i=0; i<4; i++)
//			{
//				if(extent[i]!=fileProperties->d_name[startLoc+i])
//				{
//					isEndWithTXT=0;
//					break;
//				}
//			}
//			//*************************
//
//			// eger sonu txt ile bitiyorsa okuma islemini gerceklestiriyoruz
//			if(isEndWithTXT==1)
//			{
//				//printf("%s %d\n",fileProperties->d_name,fileProperties->d_namlen);
//
//				//open file with path
//				int categorinamelen=strlen(categori_name);
//				char * path= (char *) calloc((categorinamelen+fileProperties->d_namlen+1),sizeof(char));
//				strcpy(path,categori_name);
//				path[categorinamelen]='\\';
//				strcat(path,fileProperties->d_name);
//
//				FILE* fin = fopen(path, "r");
//
//				if (fin == NULL)
//				{
//					fprintf(stderr, "Error : Failed to file input directory\n");
//							return 1;
//				}
//				//********************************************
//
//				//son dosya noduna gidiyoruz sona ekleme yapılacak
//				fnode * ftraverse;
//				if(top==NULL)
//				{
//
//
//					top=(fnode*) malloc(sizeof( fnode));
//					top->left=NULL;
//					top->right=NULL;
//					top->file=(afile*) malloc(sizeof( afile));
//					top->file->categori_type= (char *) calloc(strlen(categori_name),sizeof(char));
//					strcpy(top->file->categori_type,categori_name);
//					top->file->name= (char *) calloc(fileProperties->d_namlen,sizeof(char));
//					strcpy(top->file->name,fileProperties->d_name);
//					top->file->top=NULL;
//					top->file->wordcount=0;
//					ftraverse=top;
//				}
//				else
//				{
//					ftraverse=top;
//					while(ftraverse->right!=NULL)
//					{
//						ftraverse=ftraverse->right;
//					}
//					ftraverse->right=(fnode*) malloc(sizeof( fnode));
//					ftraverse->right->left=ftraverse;
//					ftraverse->right->right=NULL;
//					ftraverse->right->file=(afile*) malloc(sizeof( afile));
//					ftraverse->right->file->categori_type= (char *) calloc(strlen(categori_name),sizeof(char));
//					strcpy(ftraverse->right->file->categori_type,categori_name);
//					ftraverse->right->file->name= (char *) calloc(fileProperties->d_namlen,sizeof(char));
//					strcpy(ftraverse->right->file->name,fileProperties->d_name);
//					ftraverse->right->file->top=NULL;
//					ftraverse->right->file->wordcount=0;
//					ftraverse=ftraverse->right;
//
//				}
//
//				char * aword=(char *)calloc(30, sizeof(char));
//				int awordindex=0;
//				int wordstart=0;
//				char c;
//				while ((c = getc(fin)) != EOF)
//				{
//					/*if(c=='Ã')
//			    		continue;*/
//			    		if(isCharacter(c)==1)
//			    		{
//
//			    			aword[awordindex]=c;
//			    			awordindex++;
//
//			    		}
//			    		else if(c==' ' || c=='\n')
//			    		{
//			    			if(awordindex==0)
//			    				continue;
//
//			    			char nword[awordindex+1];
//			    			int i=0;
//			    			for(i=0; i<=awordindex; i++)
//			    			{
//			    				nword[i]=aword[i];
//			    			}
//			    			ftraverse->file->wordcount++;
//			    			wnode * wtraverse;
//			    			int isfind=0;
//			    			if(ftraverse->file->top==NULL)
//			    			{
//			    				ftraverse->file->top=(wnode*) malloc(sizeof( wnode));
//			    				ftraverse->file->top->left=NULL;
//			    				ftraverse->file->top->right=NULL;
//			    				ftraverse->file->top->word=(char *) calloc(awordindex,sizeof(char));
//			    				ftraverse->file->top->tf=1;
//			    				strcpy(ftraverse->file->top->word,nword);
//			    				wtraverse=top;
//			    			}
//			    			else
//			    			{
//			    				wtraverse=ftraverse->file->top;
//			    				while(wtraverse->right!=NULL)
//			    				{
//			    					if(isEqual(wtraverse->word,nword))
//			    					{
//			    						wtraverse->tf++;
//			    						isfind=1;
//			    						break;
//			    					}
//			    					wtraverse=wtraverse->right;
//			    				}
//			    				if(isfind==0)
//			    				{
//			    					wtraverse->right=(wnode*) malloc(sizeof( wnode));
//			    					wtraverse->right->right=NULL;
//			    					wtraverse->right->left=wtraverse;
//			    					wtraverse->right->word=(char *) calloc(awordindex,sizeof(char));
//			    					wtraverse->right->tf=1;
//			    					strcpy(wtraverse->right->word,nword);
//			    				}
//			    			}
//
//			    			aword= (char *)calloc(30, sizeof(char));
//			    			awordindex=0;
//			    			//printf("%s\n",nword);
//
//			    		}
//
//
//				}
//				fclose(fin);
//
//			}
//
//
//		}
//
//
//	}
//
//
//
//}
//
//void printAll()
//{
//	if(top==NULL)
//	{
//		printf("list is empty\n");
//		return;
//	}
//
//	fnode * ftraverse= top;
//	while(ftraverse!=NULL)
//	{
//		printf("<Categori: %s>  <Name: %s> <WordCount %d>\n ",ftraverse->file->categori_type,ftraverse->file->name, ftraverse->file->wordcount);
//
//		wnode * wtraverse =ftraverse->file->top;
//		while(wtraverse!=NULL)
//		{
//			printf(" %s:%d ", wtraverse->word,wtraverse->tf);
//			wtraverse=wtraverse->right;
//
//		}
//		printf("\n");
//
//		ftraverse= ftraverse->right;
//	}
//
//
//}
//
//int main(int argc, char *argv[]) {
//	//readFiles(".");
//
//	readFiles("health");
//	readFiles("magazine");
//	readFiles("sport");
//	printAll();
//	/*readFiles("magazine");
//	readFiles("sport");*/
//
//	system("pause");
//	return 0;
//}
