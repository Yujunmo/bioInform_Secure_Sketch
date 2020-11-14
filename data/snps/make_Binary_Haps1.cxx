#include <iostream>

using namespace std;

int main(){
	FILE* fp= fopen("majorAelleSnps","r");	// majorAelleSnps
	if(fp==NULL){
		cout<< "file open failure"<<endl;
		return 0;
	}
	
	char filename[30];
	char outputName[30];
	FILE* fps[10];
	FILE* fpWs[10];
	FILE* fpWsB[10];

	for(int i=0; i< 2;++i){
		fseek(fp,0L,SEEK_SET);
		sprintf(filename,"p%d",i);
		sprintf(outputName,"p_binary%d",i);
		fps[i]=fopen(filename,"r");			//p0 p1 ..
		fpWs[i]=fopen(outputName, "w");			// p_binary0 ...

		if(fps[i] ==nullptr){
			cout<<"file"<< i <<" open failure"<<endl;
			return 0;
		}
		if(fpWs[i]==nullptr){
			cout<<"file w "<<i<<" open failure"<<endl;
			return 0;
		}

		char ID_A[15];
                char ID_B[15];
                fscanf(fps[i],"%s %s",ID_A, ID_B);
                fprintf(fpWs[i],"%s\n", ID_A);
//                fprintf(fpWsB[i],"%s\t",ID_B);
                fgetc(fps[i]);		//'\n'

		while(true){
		char Major_SNP[1];

		char p_SNP_A[1];
		char p_SNP_B[1];
		
		fscanf(fp,"%s", Major_SNP);
		fscanf(fps[i],"%s %s", p_SNP_A, p_SNP_B);

		if(Major_SNP[0]==p_SNP_A[0]){
			fprintf(fpWs[i],"%d",1);	//major >> 1
		}
		else{
			fprintf(fpWs[i],"%d",0);
		}


		char tmp=fgetc(fp);
		char tmp2=fgetc(fps[i]);

		if(tmp==EOF) break;
		}

		fprintf(fpWs[i],"\n%s\n",ID_B); 
		fseek(fp,0L,SEEK_SET);
		fseek(fps[i], 0L, SEEK_SET);
		fscanf(fps[i],"%s %s",ID_A, ID_B);
		fgetc(fps[i]);

		while(true){
			char Major_SNP[1];

			char p_SNP_A[1];
			char p_SNP_B[1];

			fscanf(fp, "%s", Major_SNP);
			fscanf(fps[i], "%s %s", p_SNP_A, p_SNP_B);
			if(Major_SNP[0] == p_SNP_B[0]){
				fprintf(fpWs[i], "%d",1);
			}
			else{
				fprintf(fpWs[i], "%d",0);
			}
			char tmp= fgetc(fp);
			char tmp2= fgetc(fps[i]);
			if(tmp== EOF)
				break;
		}
		
	}
	return 0;
}
