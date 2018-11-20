#include <iostream>
#include <fstream>
#include <sstream>
#include <openssl/md5.h>
#include <string>
#include <cstring>
#include <map>
#include <algorithm>

#define MODVAL 1048576
#define CHUNKSIZE 64    //4288

using namespace std;

struct checksumValues{
    int r1,r2;
    string checksum;
};

string calcMD5(char *chunk){
    unsigned char digest[MD5_DIGEST_LENGTH];
    string checksum;
    MD5((unsigned char *)chunk, strlen(chunk), (unsigned char *)&digest);
    if(strlen((char *)digest) == 0){
		cout<<"Unable to calculate MD5 hash"<<endl;
		return NULL;
	}
    char mdString[MD5_DIGEST_LENGTH*2+1];
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
	}
    checksum = mdString;
    return checksum;
}

checksumValues calcRollingChecksum(string chunk){
    int r1=0,r2=0,r=0;
    int chunkLen = chunk.length();
    for(int i=0; i<chunkLen; i++){
        r1 += (int)chunk[i] % MODVAL;
        r2 += (((chunkLen-i+1)) * ((int)chunk[i]) ) % MODVAL;
    }
    // r2 = (r2 << 16)%MODVAL;
    r = (r1 + (r2 << 16) % MODVAL) % MODVAL;
    stringstream stream;
    stream << hex << r;
    checksumValues cv;
    cv.r1 = r1;
    cv.r2 = r2;
    cv.checksum = stream.str();
    return cv;
}

checksumValues calcIncrementalRollingChecksum(int r1, int r2, char out, char in, int chunkLen){
    checksumValues cv;
    int r = 0;
    r1 = (r1 - out + in) % MODVAL;
    r2 = (r2 - (chunkLen*out)% MODVAL + r1) % MODVAL;
    r = (r1 + (r2 << 16) % MODVAL) % MODVAL; 
    stringstream stream;
    stream << hex << r;
    cv.r1 = r1;
    cv.r2 = r2;
    cv.checksum = stream.str();
    return cv;
}

int createIndexFile(string dataFilename){
    FILE *dataFile = fopen((char *)dataFilename.c_str(), "rb");
	if(dataFile == NULL){
		cout<<"Unable to open file "<<dataFilename<<endl;
		return -1;
	}
    string indexFilename = dataFilename+".index";
    FILE *indexFile = fopen((char *) indexFilename.c_str(), "wb");
    if(indexFile == NULL){
        cout<<"Unable to create index file for data file: "<<dataFilename<<endl;
        return -1;
    }
    int index = 0;
    char chunk[CHUNKSIZE+1];
    string weakChecksum, strongChecksum;
    checksumValues cv;
    while(fgets(chunk, CHUNKSIZE, dataFile)){
        cv = calcRollingChecksum(chunk);
        strongChecksum = calcMD5(chunk);
        fprintf(indexFile, "%s:%s:%d\n",(char *)cv.checksum.c_str(), (char *)strongChecksum.c_str(), index);
        index++;
    }
}

int main(){
    /*string checksum = "";
    char input[CHUNKSIZE+1];
    string filename = "df.txt";
    FILE *file = fopen((char *)filename.c_str(), "rb");
	if(file == NULL){
		cout<<"Unable to open file "<<filename<<endl;
		return 0;
	}
    int cnt = 0;
    map<string, int> m;
    while(fgets(input , CHUNKSIZE , file)){
        // fseek(file, CHUNKSIZE, SEEK_CUR);
        cnt++;
        checksumValues cv = calcRollingChecksum(input);
        checksum = cv.checksum;
        auto it = m[checksum];
        if(it == 0){
            m.insert({checksum, 1});
        }else{
            m[checksum]++;
        }
        cout<<checksum<<endl;
        memset(input, 0, sizeof(input));
    }
    fread.close();
    cout<<"Total chunks are: "<<cnt<<endl;
    cout<<"Map size is: "<<m.size()<<endl;
    */
    createIndexFile("2.out");
    return 0;
}