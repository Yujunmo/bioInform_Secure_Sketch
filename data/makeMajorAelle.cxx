#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
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

int main(int argc, char** argv)
{
	FILE* fp0 = fopen(argv[1], "r");
	FILE* fp1 = fopen("./majorAelleFile", "w");

	skipFirstLine(argv[1], fp0);


	if (fp0 == nullptr || fp1 == nullptr) {
		cout << "file open failure" << endl;
		return 0;
	}

	char rsID[20];
	char pos[20];
	char A[3];
	char B[3];
	int stop=0;
	int lines = 0;
	while (true) {
//		printf("%d ", lines++);
		fscanf(fp0, "%s %s", rsID, pos);
//		printf("%s\t%s\t", rsID, pos);
		fprintf(fp1, "%s\t%s\t", rsID, pos);
		int A_CNT[4] = { 0 };
		int B_CNT[4] = { 0 };
		while (true){

			fscanf(fp0, "%s %s", A, B);
//			printf("%s, %s ", A, B);
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
				fgetc(fp0);	// 10 

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
	//	printf("%c ", majorAelle);

		if(stop==1){
			break;
		}

	}

	fclose(fp0);
	fclose(fp1);
	return 0;
}
