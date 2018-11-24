#include <openssl/sha.h>
#include <bits/stdc++.h>

using namespace std;


int main(){
	string filename;
	cout<<"Enter FileName: ";
	cin>>filename;

	char file[filename.length()+1];
    strcpy(file,filename.c_str());

    FILE *fp = fopen(file,"rb");
    if(fp==NULL){

        printf("File open error");
        return 1;   
    }
    FILE *fpd = fopen("sha_digest","a");
    if(fpd==NULL){
        printf("File open error");
        return 1;   
    }
    int count=1;
    char data[524288]={0};

    while(fgets (data , 524288 , fp)){
    	fseek ( fp , 524288 , SEEK_CUR );	
		cout<<count++<<"\n";
	    char buf[21];
	    size_t length = 524288;
		unsigned char hash[SHA_DIGEST_LENGTH]={'\0'};
		
		SHA1((unsigned char*)data, length, (unsigned char*)&hash);
		for (int i=0; i < 10; i++) {
			sprintf((char*)&(buf[i*2]), "%02x", hash[i]);
		}	
		//buf[21] = '\n';
		fputs (buf,fpd);
		memset(data,0,524288);
    }
	return 0;
}

/*

string computeSHAforChunk(char *chunk){//string str){
	unsigned char digest[SHA_DIGEST_LENGTH];
	string retHash;
	SHA1((unsigned char*)chunk, strlen(chunk), (unsigned char*)&digest);
	//SHA1((unsigned char*)str.c_str(), str.length(), (unsigned char*)&digest);
	if(strlen((char *)digest) == 0){
		cout<<"Unable to calculate hash"<<endl;
		return NULL;
	}
   char mdString[SHA_DIGEST_LENGTH*2+1];

   for(int i = 0; i < SHA_DIGEST_LENGTH; i++){
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
	}
	retHash = mdString;
	return retHash;
}*/