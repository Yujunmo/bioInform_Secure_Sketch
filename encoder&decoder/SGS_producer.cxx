#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

int findMax(int* cnt) {
        int max_index = 0;
        int max = cnt[0];
        for (int i = 1; i < 4; ++i) {
                if (max < cnt[i]) {
                        max = cnt[i];
                        max_index = i;
                }
        }
        return max_index;
}

void skipFirstLine(char * file, FILE * fp){
        char tmp;

        while(true){
                tmp=fgetc(fp);
                if(tmp==13)
                        break;
        }
        fgetc(fp); // remove 10
}


void makeMajorAelle(char * hapmapData ){

       	FILE* fp0 = fopen(hapmapData, "r");
        FILE* fp1 = fopen("./majorAelleFile", "w");

        skipFirstLine(hapmapData, fp0);

        if (fp0 == nullptr || fp1 == nullptr) {
                cout << "file open failure" << endl;
                return;
        }

        char rsID[20];
        char pos[20];
        char A[3];
        char B[3];
        int stop=0;
        int lines = 0;

        while (true) {
                fscanf(fp0, "%s %s", rsID, pos);
                fprintf(fp1, "%s\t%s\t", rsID, pos);
                int A_CNT[4] = { 0 };
                int B_CNT[4] = { 0 };
                while (true){

                        fscanf(fp0, "%s %s", A, B);
                        if (A[0] == 'A') A_CNT[0]++;
                        else if (A[0] == 'C') A_CNT[1]++;
                        else if (A[0] == 'G') A_CNT[2]++;
                        else if (A[0] == 'T') A_CNT[3]++;

                        if (B[0] == 'A') B_CNT[0]++;
                        else if (B[0] == 'C') B_CNT[1]++;
                        else if (B[0] == 'G') B_CNT[2]++;
                        else if (B[0] == 'T') B_CNT[3]++;

			char tmp = fgetc(fp0);  // tab 처리
                        tmp = fgetc(fp0);
                        if (tmp == 13) {
                                fgetc(fp0);     // 10

                                char tmp2= fgetc(fp0);
                                if(tmp2 == EOF){
                                        stop=1;
                                        break;
                                }
                                else{
                                        ungetc(tmp2,fp0);
                                }
                        break;
                        }
                        else {
                        ungetc(tmp, fp0);
                        }
                }
                A_CNT[0] += B_CNT[0];
                A_CNT[1] += B_CNT[1];
                A_CNT[2] += B_CNT[2];
                A_CNT[3] += B_CNT[3];

                char majorAelle;
                int max= findMax(A_CNT);
                if (max == 0) majorAelle = 'A';
                else if (max == 1) majorAelle = 'C';
                else if (max == 2) majorAelle = 'G';
                else  majorAelle = 'T';

                fprintf(fp1, "%c\t\n", majorAelle);
                if(stop==1){
                        break;
                }

        }
        fclose(fp0);
        fclose(fp1);
        return;
}

int findbyID(char* hapmapData, char* ID ){

	FILE* fp = fopen(hapmapData, "r");
	char personID[30];
        char * person;
        int count=3;

        if (fp == nullptr) {
                cout << "file open failure" << endl;
                return -2;
        }

	fscanf(fp,"%s",personID);
        fscanf(fp,"%s",personID);

        while(1){
                fscanf(fp,"%s", personID);
                person=strtok(personID,"_");
                if(strcmp(person,ID)==0){
			fclose(fp);
			return count;
		}
        else{
                count++;
                char tmp;
                tmp=fgetc(fp);
                if(tmp=32){
                        fscanf(fp,"%s",personID);
                }
                fgetc(fp);	 //tab
                tmp=fgetc(fp);
                if(tmp==13){
			fclose(fp);
                        return -1; //counldn't find such person
                }
                else{
                        ungetc(tmp,fp);
                }
	}

      }
}


// input  :  hapmapdata, NA19051. 
// output :  make majorAelles (if doesn't exist)
// 	     cut NA19051 snps by column. p0
void readInputData(char *hapmapData, char* ID){

	//++ hash >> reduce unnecessary calculation 
	makeMajorAelle(hapmapData);
	int col = findbyID(hapmapData, ID);
	
	if(col==-1){
		printf("couldn't find the person under such ID\n");
		exit(1);
	}
	if(col==-2){
		exit(1);

	}
	char command[100];
	sprintf(command, "cut -f %d ./%s > ./p0",col,hapmapData); //++ avoid overlap of filename
	system(command);
}

// compare p0 and majorAelle and convert p0 to binary file
int  makeHaps2Binary(){
	system("cut -f 3 ./majorAelleFile > ./majorAelleSnps");
	FILE* fp= fopen("majorAelleSnps","r");  // majorAelleSnps
        if(fp==NULL){
                cout<< "file open failure"<<endl;
                return -1;
        }

        char filename[30];
        char outputName[30];
        FILE* fps[10];
        FILE* fpWs[10];
        FILE* fpWsB[10];

        for(int i=0; i< 1;++i){
                fseek(fp,0L,SEEK_SET);
                sprintf(filename,"p%d",i);
                sprintf(outputName,"p_binary%d",i);
                fps[i]=fopen(filename,"r");                     // p0 p1 ..
                fpWs[i]=fopen(outputName, "w");                 // p_binary0 ...

                if(fps[i] ==nullptr){
                        cout<<"file"<< i <<" open failure"<<endl;
                        return -1;
                }
                if(fpWs[i]==nullptr){
                        cout<<"file w "<<i<<" open failure"<<endl;
                        return -1;
                }

                char ID_A[15];
                char ID_B[15];
                fscanf(fps[i],"%s %s",ID_A, ID_B);
                fprintf(fpWs[i],"%s\t", ID_A);
                fgetc(fps[i]);          //'\n'

                while(true){
                char Major_SNP[1];

                char p_SNP_A[1];
                char p_SNP_B[1];

                fscanf(fp,"%s", Major_SNP);
                fscanf(fps[i],"%s %s", p_SNP_A, p_SNP_B);

                if(Major_SNP[0]==p_SNP_A[0]){
                        fprintf(fpWs[i],"%d",1);        //major >> 1
                }
                else{
                        fprintf(fpWs[i],"%d",0);
                }

		                char tmp=fgetc(fp);
                char tmp2=fgetc(fps[i]);

                if(tmp==EOF) break;
                }

                fprintf(fpWs[i],"\n%s\t",ID_B);
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

// convert binary file to GS 
void convertBinary2GS(char* ID ){
	char command[100];
	sprintf(command,"./convertHap2Set -i p_binary0 -o %s.set",ID);
	system(command);
}

// GS >> SGS 
void convertGS2SGS(char* ID ){
        char gsFILE[20];
        sprintf(gsFILE,"%s.set",ID);
        FILE* fp = fopen(gsFILE,"r");
        if(fp== nullptr){
                cout<<"failed to open"<<endl;
                exit(1);
        }
        int x;
        int element;
        
        char* gsVector= new char[(int)pow(2,24)];
        for(int i=0; i<(int)pow(2,24);++i){
                gsVector[i]='0';
        }

        while(true){
                x= fscanf(fp,"%d",&element);
                if(x==-1){
                        break;
                }
                gsVector[element]='1';
        }

	//confirmed
	// character , integer needs to be distinguished

        FILE* fp2= fopen("matrix.txt","r");
        char** codewords = new char*[5];
        for(int i=0; i<5;++i){
                codewords[i]= new char[(int)pow(2,24)];
        }

        for(int i=0; i<5;++i){
                fscanf(fp2, "%s", codewords[i]);
        }


        int randomRow = rand()%5;

        for(int i=0; i< (int)pow(2,24); ++i){
                if(gsVector[i]== codewords[randomRow][i]){
                        gsVector[i]='0';
                }
                else{
                        gsVector[i]='1';
                }
        }

        char sgs[20];
        sprintf(sgs,"SGS_%s.set",ID);
        FILE* fp3 = fopen(sgs,"w");
        if(fp3==nullptr){
                cout<<"failed"<<endl;
                exit(1);
        }

	
        for(int i=0; i<(int)pow(2,24);++i){
                if(gsVector[i]=='1'){
			fprintf(fp3,"%d ",i);
                }
        }

        fclose(fp);
        fclose(fp2);
        fclose(fp3);
        for(int i=0; i<5;++i){
                delete codewords[i];
        }
        delete[] codewords;
        
}


// argv[1] = hapmap data file, argv[2] = ID (NA1904)
int main(int argc, char** argv){

	if(argc!=3){
		printf("command format : ./GS_producer [hapmapdatafile] [personID]\n");
		return 0;
	}

	readInputData(argv[1], argv[2]);  //make majorAelle, p0 file
	if(-1==makeHaps2Binary()){	  //make binary file
		exit(1);
	}

	convertBinary2GS(argv[2]);	  //convert binaryfile to GS
	convertGS2SGS (argv[2]);
	return 0;

}
