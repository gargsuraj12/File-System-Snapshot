#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <openssl/sha.h>
#include <openssl/md5.h>

using namespace std;

#define CHUNKSIZE 524288

vector<string> splitLine(string line, string delimiter){
    vector<string> splitV;
    string token;
    int index = line.find(delimiter);
    while (index != -1)
    {
        token = line.substr(0, index);
        splitV.push_back(token);
        line.erase(0, index + 1);
        index = line.find(delimiter);
    }
    if (!line.empty())
    {
        splitV.push_back(line);
    }
    return splitV;
}

void getFileHash(char *fileName){

    unsigned char result[2 * SHA_DIGEST_LENGTH];
    unsigned char hash[SHA_DIGEST_LENGTH];
    int i;
    FILE *f = fopen(fileName, "rb");
    SHA_CTX mdContent;
    int bytes;
    unsigned char data[1024];

    if (f == NULL)
    {
        printf("%s couldn't open file\n", fileName);
        exit(1);
    }

    SHA1_Init(&mdContent);
    while ((bytes = fread(data, 1, 1024, f)) != 0)
    {

        SHA1_Update(&mdContent, data, bytes);
        for (i = 0; i < SHA_DIGEST_LENGTH; i++)
        {
            printf("%02x", hash[i]);
        }
        printf("\n");
        /** if you want to see the plain text of the hash */
        for (i = 0; i < SHA_DIGEST_LENGTH; i++)
        {
            sprintf((char *)&(result[i * 2]), "%02x", hash[i]);
        }

        printf("%s\n", result);
    }

    SHA1_Final(hash, &mdContent);

    fclose(f);
}

string calcHash(char *chunk){
    // unsigned char digest[SHA_DIGEST_LENGTH+1] = {0};
    unsigned char digest[MD5_DIGEST_LENGTH+1] = {0};
	string retHash;
    size_t length = strlen(chunk);
    // if(length <= 0){
    //     cout<<"Hash cannot be calculated for empty string.."<<endl;
    //     return "";
    // }
	// SHA1((unsigned char*)chunk, length, (unsigned char*)&digest);
    MD5((unsigned char *)chunk, length, (unsigned char *)&digest);
	// if(strlen((char *)digest) == 0){
	// 	cout<<"Unable to calculate hash"<<endl;
	// 	return "";
	// }
    // char mdString[(SHA_DIGEST_LENGTH*2)+1] = {0};
   char mdString[(MD5_DIGEST_LENGTH*2)+1] = {0};
   for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
        sprintf((char *)&mdString[i*2], "%02x", (unsigned int)digest[i]);
	}
	retHash = mdString;
    memset(mdString, 0, sizeof(mdString));
    memset(digest, 0, sizeof(digest));
	return retHash;
}




int main(){

    string filename = "Linux_bak.pdf.index";
    FILE *fp = fopen((char *)filename.c_str(), "rb");
    if(fp == NULL){
        cout<<"Unable to open- "<<filename<<endl;
        return -1;
    }
    char *line = NULL;
    size_t len = 0;
    while(getline(&line, &len, fp) != -1){
        cout<<line;
    }
    fclose(fp);

    // cout<<"SHA_DIGEST_LENGTH: "<<SHA_DIGEST_LENGTH<<endl;
    // string filename = "/home/suraj/Desktop/Movies/BhaagMilkhaBhaag.mkv";
    // FILE *fp = fopen((char *)filename.c_str(),"rb");
    // if(fp==NULL){
    //     printf("File open error");
    //     return -1;   
    // }
    // unsigned int i=1;
    // char data[CHUNKSIZE+1]={0};
    // while(fread(data, 1, CHUNKSIZE, fp) > 0){//fgets (data , CHUNKSIZE , fp)
    //     string hash = calcHash(data);
    //     cout<<i<<". SHA1 hash is: "<<hash<<endl;
	// 	memset(data,0,sizeof(data));
    //     i++;
    // }
    // fclose(fp);

    // getFileHash("Id.jpg");
    // FILE *fr, *fw;
    // char block[100]= {0};
    // fr = fopen("df.txt.bak", "rb");
    // fw = fopen("temp.txt", "wb");
    // int x = fread(block, 1, 64, fr);
    // while(x>0){
    //     cout<<x<<" "<<block<<endl;
    //     fwrite(block, 1, x, fw);
    //     memset(block, 0, 100);
    //     x = fread(block, 1, 64, fr);
    // }

    // fclose(fr);
    // fclose(fw);

    // int n;
    // string data;
    // cout<<"Enter the Number of records to be written to the file: "<<endl;
    // cin>>n;
    // cin.ignore();
    // ofstream fwrite;
    // fwrite.open("records.dat", ios::binary);
    // for(int i=1; i<=n; i++){
    //     cout<<"Enter name: ";
    //     getline(cin, data);
    //     fwrite<<data<<"|";
    //     cout<<"Enter age: ";
    //     getline(cin, data);
    //     fwrite<<data<<"\n";
    // }
    // fwrite.close();

    // ifstream fread;
    // fread.open("records.dat", ios::binary);
    // vector<string> line;
    // getline(fread, data);
    // while(fread){
    //     line = splitLine(data, "|");
    //     cout<<line[0]<<"\t"<<line[1]<<endl;
    //     getline(fread, data);
    // }
    // fread.close();
    return 0;
}