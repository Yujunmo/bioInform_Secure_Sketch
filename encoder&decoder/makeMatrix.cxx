#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;

int main(){
    FILE* fp = fopen("matrix.txt","w");

    if(fp==nullptr){
        cout<<"hello"<<endl;
        return -1;
    }

    char * arr= new char[(int)pow(2,24)];
    for(int i=0; i<5; ++i){

        for(int j=0; j<(int)pow(2,24);++j){
            arr[j]= rand()%2;
            fprintf(fp,"%d",arr[j]);
        }
        fprintf(fp,"\n");
    }


    delete arr;
    fclose(fp);
    return 0;
}