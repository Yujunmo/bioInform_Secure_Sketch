/*
 * crypto.cxx
 * 
 * Copyright 2013 Farhad <farhad@farhad-ThinkPad-W520>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm> 
using namespace std;

#include <openssl/sha.h>

#define SEGMENT_SIZE 300
#define SALT_SIZE 100

int convertBits2Int(int *src, int size);

string sha256(const string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

string convertBits2Strings(int *src, int size) {
	string result = "";
	int i = size;
	while(i - 8 > 0) {
		int value = convertBits2Int(src+i-8, 8);
		result = (char)value + result;
		i = i - 8;
	}
	if(i > 0) {
		int value = convertBits2Int(src, i);
		result = (char)value + result;
	}
	return result;
}

int convertHexString2Int(string hex) {
	string hexString = hex;
	int hexNumber;
	sscanf(hexString.c_str(), "%x", &hexNumber);
	return hexNumber;
}

void printBits(int *src, int size) {
	for(int i = 0 ; i < size; i++) {
		printf("%d ", src[i]);
	}
	printf("\n");
}

void randomBits(int * dest, int rand_size, int size) {
		for(int i = size-1; i > size-rand_size; i--) 
			dest[i] = rand() % 2;
		for(int i = size-rand_size; i > 0; i--)
			dest[i] = 0;
}

void convertInt2Bits(int *dest, unsigned int segmentNum, unsigned int size) {
	unsigned int value = segmentNum;
	for(int i = size-1; i >=0; i--) {
		dest[i] = value % 2;
		value /= 2;
	}
}
// bit >> stirng ex) 101 >> 5
int convertBits2Int(int *src, int size) {
	unsigned int value = 0;
	for(int i = 0; i < size; i++) {
		value = value * 2 + src[i];
	}
	return value;
}

void addBinary(int *src1, int *src2, int *dest, int size) {
	int carry = 0;
	for(int i = size-1; i >= 0 ; i--) {
		dest[i] = (src1[i] + src2[i] + carry) % 2;
		carry   = (src1[i] + src2[i] + carry >= 2 ? 1 : 0);
	}
}

/*We remove the white space from the string*/
void trimStrings(string & data) {
	data.erase(std::remove(data.begin(), data.end(), ' '), data.end());
	data.erase(std::remove(data.begin(), data.end(), '\t'), data.end());
}

void writeSaltBits2File(int *saltBits, int size, ofstream & outputFILE) {
	for(int i = 0; i < size; i++) {
		outputFILE << saltBits[i];
	}
	outputFILE << endl;
}

void readSaltBitsFromFile(int *saltBits, int size, ifstream & inputFILE) {
	string line;
	std::getline(inputFILE, line);
	trimStrings(line);
	for(int i = 0; i < line.size(); i++)
		saltBits[i] = line[i]-'0';
}

void convertGenome2Bits(string & genome, int * genomeBits, int segment_num) {
	int genome_size = genome.size();
	for(int i = segment_num*SEGMENT_SIZE; ( (i < (segment_num+1)*SEGMENT_SIZE) && (i < genome_size) ); i++ ) {
		//genomeBits[i-segment_num*SEGMENT_SIZE] = genome[i] - '0';
		if(genome[i]=='1')
			genomeBits[i-segment_num*SEGMENT_SIZE]=0;		// ?
		else 
			genomeBits[i-segment_num*SEGMENT_SIZE]=1;
	}
}

// heqseq은 전체 길이지만 여기서는 세그먼트 단위별 데이터처리
// 비트를 진짜 비트로 표현하지 않음. 인트형 배열로 표현함. 인트형 배열로 표현하고 비트라고 생각하기
int genomeSketchElement(string & genome, int segment_num, int * saltBits) {
	int * genomeBits = new int[SEGMENT_SIZE+1];
	int * tmpBits1    = new int[SEGMENT_SIZE+1];
	int * tmpBits2    = new int[SEGMENT_SIZE+1];
	int * segmentPositionBists = new int[SEGMENT_SIZE+1];
	
	convertGenome2Bits(genome, genomeBits, segment_num);			//string지놈의 한 세그먼트를 비트로 바꾸기 (인트형 배열)
	convertInt2Bits(segmentPositionBists, segment_num, SEGMENT_SIZE);	//세그먼트의 위치(인트)를 비트로 바꾸기    (인트형 배열)	
	//printBits(genomeBits, SEGMENT_SIZE);
	addBinary(genomeBits, segmentPositionBists, tmpBits1, SEGMENT_SIZE);	//위 두개를 더해서 tmpbit1에 저장
	addBinary(tmpBits1, saltBits, tmpBits2, SEGMENT_SIZE);			//tmpbits에 saltbits(random bit) 더하기. (진짜 비트간 연산아님. 인트형 배열간 연산임)
	string tmpSketch = convertBits2Strings(tmpBits2, SEGMENT_SIZE);		//
	string sketchElement = sha256(tmpSketch);				//해싱하기

	int valueSketchElement = convertHexString2Int(sketchElement.substr(0,6));//앞 6자리 잘라오기. 6digit hexdecimal >> 10 decimal string

	delete [] segmentPositionBists;
	delete [] genomeBits;
	delete [] tmpBits1;
	delete [] tmpBits2;
	return valueSketchElement;
}

void encrypteGenotype(string genome_hap1, string genome_hap2, int * saltBits, ofstream & outputFILE) {
	int theshould   = 0;
	int non_zero    = 0;
	int bitUsed     = 24;
	int genome_size = genome_hap1.size();
	
	char * genomeSketchVector = new char[(int)pow(2,24)];
	for(int i = 0; i < (int)pow(2,24); i++)
		genomeSketchVector[i] = '0';
	for(int i = 0; i < genome_size/SEGMENT_SIZE; i++) {
		int value = genomeSketchElement(genome_hap1, i, saltBits);	//process data by segment 
		genomeSketchVector[value] = '1';		//segment 데이터 처리해서 나온 값을 이제 정수값으로 나타낸게 value. 해당 벡터에 있음을 표시.?(논문이랑 매치되나..?)
	}
	
	for(int i = 0; i < genome_size/SEGMENT_SIZE; i++) {
		int value = genomeSketchElement(genome_hap2, i, saltBits);
		genomeSketchVector[value] = '1';
	}							//gs vector complete

	for(int i = 0; i < (int)pow(2,24); i++) {
		if(genomeSketchVector[i] == '1')
			non_zero = non_zero + 1;
	}
	theshould   = 2 * non_zero - 2*5000;			// ?
	outputFILE << "t=" << theshould << endl;
	outputFILE << "m=" << bitUsed    << endl;
	outputFILE << "[" ;
	for(int i = 0; i < (int)pow(2,24); i++) {		// 1의 인덱스 출력(pinSketchFile)
		if(genomeSketchVector[i] == '1')
			outputFILE << i << " ";
	}
	outputFILE << "]" << endl;
	delete [] genomeSketchVector;
}
	
int main(int argc, char **argv) {
	int v1 = 0;
	int v2 = 0;
	string hapSeq1;
	string hapSeq2;
	string hapName;	
	int count = 0;
	int * saltBits   = new int[SEGMENT_SIZE+1];
	string hapMapFileName = "NA06989.in";
	string saltFileName   = "salt.pub";
	string outputFileName = "output.out";

	for(int i = 0; i < argc; i++) {
                if(strcmp(argv[i], "-i") == 0)   //sam mapping file
                        v1 = i+1;
                if(strcmp(argv[i], "-o") == 0)
                        v2 = i+1;
                if( (strcmp(argv[i], "-h") == 0) || 
		    (strcmp(argv[i], "--h") == 0) || 
 		    (strcmp(argv[i], "-help") == 0) ||
		    (strcmp(argv[i], "--help") == 0) ) {
                        cout << "It generate the set for each individuals need as input for the Improved Jule sudan program" << endl;
                        cout << "Parameters" << endl;
                        cout << "-i the input file name" << endl;
                        cout << "-o the output file name" << endl;

                        cout << "Example ./convertHap2Set -i NA06989.out -o NA06989.set" << endl;
                        exit(0);
                }

        }

	if(v1 == 0 || v2 == 0) {
		cout << "Your input or output file name is not right!!!" << endl;
		exit(0);
	}

	hapMapFileName = string(argv[v1]);
	outputFileName = string(argv[v2]);	
        ifstream hapmapDataFILE(hapMapFileName.c_str());
	//Check if salt key exist or not
	ifstream saltPublicFILE(saltFileName.c_str());
	if(saltPublicFILE.fail()) {
		ofstream saltPublicFILE(saltFileName.c_str(), std::ofstream::out);
		srand(time(NULL));
		randomBits(saltBits, SALT_SIZE, SEGMENT_SIZE);
		printBits(saltBits, SEGMENT_SIZE);
		writeSaltBits2File(saltBits, SEGMENT_SIZE, saltPublicFILE);
		saltPublicFILE.close();
	} else {
		saltPublicFILE.close();
		cout << "START LOADING SALT" << endl;
		saltPublicFILE.open(saltFileName.c_str());
		if(saltPublicFILE.is_open()) {
			readSaltBitsFromFile(saltBits, SEGMENT_SIZE, saltPublicFILE);
			printBits(saltBits, SEGMENT_SIZE);
			cout  << "FINISH LOADING SALT" << endl;
			saltPublicFILE.close();
		}
	}
	//Salt random bits is load or generated
	
	while (std::getline(hapmapDataFILE, hapSeq1)) {
		int size_line = hapSeq1.size();
		int first_tab = hapSeq1.find("\t");
		
		hapName = hapSeq1.substr(0, first_tab-1); 
		hapSeq1  = hapSeq1.substr( first_tab,  size_line-first_tab);  
		ofstream pinSketchFILE(outputFileName.c_str(), std::ofstream::out);
		trimStrings(hapSeq1);
		
		std::getline(hapmapDataFILE, hapSeq2);
		size_line = hapSeq2.size();
		first_tab = hapSeq2.find("\t");
		hapSeq2   = hapSeq2.substr( first_tab,  size_line-first_tab);
		trimStrings(hapSeq2);

		encrypteGenotype(hapSeq1, hapSeq2, saltBits, pinSketchFILE);
		
		count = count + 1;
		hapSeq1="";
		hapSeq2="";
	}
	delete [] saltBits;
}

