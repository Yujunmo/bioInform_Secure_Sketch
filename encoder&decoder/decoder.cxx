#include <iostream>
#include <math.h>
using namespace std;

int main(int argc, char** argv){

    if(argc!=3){
        cout<<"format : ./decoder [SGS file] [your GS file]"<<endl;
        return -1;
    }

    FILE* SGS_FILE = fopen(argv[1],"r");
    FILE* GS_FILE = fopen(argv[2],"r");

    if(SGS_FILE == nullptr || GS_FILE == nullptr){
        cout<< "file open failure"<<endl;
        return -1;
    }

    char* SGS_vector = new char[(int)pow(2,24)];
    char*  vector = new char[(int)pow(2,24)];
    
    for(int i=0; i< (int)pow(2,24);++i){
        SGS_vector[i]=0;
        vector[i]=0;
    }

    int element=0;
    int end;

    //read SGS
    while(true){
        end=fscanf(SGS_FILE,"%d",&element);
        if(end==-1){
            break;
        }
        SGS_vector[element]=1;
    }
    element=0;

    //read GS
    while(true){
        end= fscanf(GS_FILE, "%d", &element);
        if(end==-1){
            break;
        }
        vector[element]=1;
    }

    for(int i=0; i<(int)pow(2,24);++i){
        if(SGS_vector[i]== vector[i]){
            vector[i]='0';
        }
        else{
            vector[i]='1';
        }
    }

    FILE* matrix_FILE= fopen("matrix.txt","r");

    
    int count;
    char* matrix = new char[(int)pow(2,24)];
    for(int i=0;i<5;++i){
        count=0;
        fscanf(matrix_FILE,"%s",matrix);
        for(int j=0; j<(int)pow(2,24);++j){
            if(vector[j]!= matrix[j]){
                count++;
            }
	    /*
            if(count>=500){
                cout<<i<<"th row is out of hamming distance"<<endl;
                matrix[0]=0;
                break;
            }
	    */
        }
	cout<<"number of mismatch at"<<i<<"th row :"<<count<<endl;
	
	/*
        if(count<500){
            cout<<i<<"th row is  sucessfully matched."<< "mistmatch: "<<count<<endl;
        }
	*/
    }
    
    fclose(SGS_FILE);
    fclose(GS_FILE);
    fclose(matrix_FILE);
    delete SGS_vector;
    delete vector;
    delete matrix;
    return 0;
}
