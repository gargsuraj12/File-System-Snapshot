#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <openssl/sha.h>

using namespace std;

vector<string> splitLine(string line, string delimiter)
{
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

void getFileHash(char *fileName)
{

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

int main()
{
    getFileHash("Id.jpg");
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