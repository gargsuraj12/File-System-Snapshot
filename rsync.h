#include <iostream>
#include <fstream>
#include <sstream>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <string>
#include <cstring>
#include <map>
#include <algorithm>
#include <vector>
#include <unistd.h>

using namespace std;
//#define MODVAL 1048576
#define MODVAL 65521
// static const int CHUNKSIZE  = 40;
static const string DELIM = "$";

// map<string, pair<string, int>> indexMap;

struct checksumValues{
    int r1,r2;
    string checksum;
};


class Rsync{

    public:

    map<string, pair<string, int>> indexMap;
    

    //Calculates MD5 hash for the given data chunk
    vector<string> splitLine(string line, string delimiter){
        vector<string> splitV;
        string token;
        int index = line.find(delimiter);
        while(index != -1){
            token = line.substr(0, index);
            splitV.push_back(token);
            line.erase(0, index+1);
            index = line.find(delimiter);
        }
        if(!line.empty()){
            splitV.push_back(line);
        }
        return splitV;
    }

    //check the current line contains a valid index
    int checkValidIndex(string line, string delimeter){
        vector<string> vec = splitLine(line, delimeter);
        // cout<<"vector size after splitting is: "<<vec.size()<<endl;
        if(vec.size() != 3){
            return -1;
        }
        if(vec[0] != ""){
            return -1;
        }
        return stoi(vec[1]);
    }

    //Calculates MD5 hash for the given data chunk
    string calcMD5(char *chunk){
        unsigned char digest[MD5_DIGEST_LENGTH+1] = {0};
        string retHash;
        size_t length = strlen(chunk);
        MD5((unsigned char *)chunk, length, (unsigned char *)&digest);
        char mdString[(MD5_DIGEST_LENGTH*2)+1] = {0};
        for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
            sprintf((char *)&mdString[i*2], "%02x", (unsigned int)digest[i]);
        }
        retHash = mdString;
        memset(mdString, 0, sizeof(mdString));
        memset(digest, 0, sizeof(digest));
        return retHash;
    }

    //calculates weak checksum for the given data chunk and return values of a, b and checksum
    checksumValues calcRollingChecksum(string chunk){
        int r1=0,r2=0,r=0;
        int chunkLen = chunk.length();
        for(int i=0; i<chunkLen; i++){
            r1 += (int)chunk[i] % MODVAL;
            //r2 += (((chunkLen-i+1)) * ((int)chunk[i]) ) % MODVAL;
            r2 += (((chunkLen-i)) * ((int)chunk[i]) ) % MODVAL;
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

    //calculate increamental rolling checksum using previous checksum
    //outPosition - is index which is moved out of the window 
    checksumValues calcIncrementalRollingChecksum(int r1, int r2, char outChar, char inChar, int chunkLen ,int outPosition){
        checksumValues cv;
        int r = 0;
        
        //r1 = (r1 - out + in) % MODVAL;
        //r2 = (r2 - (chunkLen*out)% MODVAL + r1) % MODVAL;


        r1 = (r1 - outChar + inChar) % MODVAL;
        r2 = (r2 - ((chunkLen-outPosition)*outChar)% MODVAL + r1) % MODVAL;
        r = (r1 + (r2 << 16) % MODVAL) % MODVAL; 

        stringstream stream;
        stream << hex << r;
        cv.r1 = r1;
        cv.r2 = r2;
        cv.checksum = stream.str();
        return cv;
    }

    string prepareIndexOfBackupFile(string dataFilename, int chunkSize){
        indexMap.clear();
        cout<<"--Inside prepareIndexOfBackupFile()--"<<endl;
        FILE *dataFile = fopen((char *)dataFilename.c_str(), "rb");
        if(dataFile == NULL){
            cout<<"Unable to open file "<<dataFilename<<endl;
            return "";
        }
        string indexFilename = dataFilename+".index";
        FILE *indexFile = fopen((char *) indexFilename.c_str(), "wb");
        if(indexFile == NULL){
            cout<<"Unable to create index file for data file: "<<dataFilename<<endl;
            return "";
        }
        int index = 0;
        char chunk[chunkSize+1]= {0};
        string weakChecksum, strongChecksum;
        checksumValues cv;
        int j=0;
        char ch;
        bool eof = false;
        while(!eof){
            int x = fread(chunk, 1, chunkSize, dataFile);
            cout<<"Bytes read from backup file are: "<<x<<endl;
            if(x<=0){   //ch == EOF
                eof = true;
                continue;
            }
            cout<< j++ <<"th. chunk of backup file is: "<<chunk<<endl;
            cv = calcRollingChecksum(chunk);
            strongChecksum = calcMD5(chunk);
            fprintf(indexFile, "%s:%s:%d\n",(char *)cv.checksum.c_str(), (char *)strongChecksum.c_str(), index);
            indexMap.insert({cv.checksum, {strongChecksum, index}});
            index++;
            memset(chunk, 0, chunkSize+1);
        }
        fclose(dataFile);
        fclose(indexFile);
        return indexFilename;
    }

    int checksumMatch(string indexFile, string chunk){
        FILE *iPtr = fopen((char *) indexFile.c_str(), "r");
        if(iPtr == NULL){
            cout<<"Unable to open- "<<indexFile<<endl;
            return -1;
        }
        char *line = NULL;
        size_t len = 0;
        string weakChecksum, strongChecksum;
        string indexStr;
        int index;
        pair<string, int> iPair;
        weakChecksum = calcRollingChecksum(chunk).checksum;
        iPair = indexMap[weakChecksum];
        if(iPair.first == ""){
            fclose(iPtr);
            return -1;
        }
        strongChecksum = calcMD5((char *)chunk.c_str());
        if(strongChecksum == iPair.first){
            fclose(iPtr);
            return iPair.second;
        }
        fclose(iPtr);
        return -1;
    }

    string prepareUpdateIndexFile(string srcDataFile, string indexFile, int chunkSize){
        cout<<"--Inside prepareUpdateIndexFile()--"<<endl;
        FILE *dPtr, *uPtr;
        dPtr = fopen((char *)srcDataFile.c_str(), "rb");
        if(dPtr == NULL){
            cout<<"Unable to open data file.."<<endl;
            return "";
        }
        string updateFile = srcDataFile+".updateIndex";
        uPtr = fopen((char *)updateFile.c_str(), "wb");
        if(uPtr == NULL){
            cout<<"Unable to create update file.."<<endl;
            return "";
        }
        char chunk[chunkSize+1] = {0};
        string weakChecksum, strongChecksum;
        int index;
        fpos_t position;
        fgetpos(dPtr, &position);
        fsetpos(dPtr, &position);
        int i=0;
        bool eof = false;
        char dummy = '#';
        while(!eof){
            // cout<<"at beginning --- Source file head is at: "<<ftell(dPtr)<<endl;
            int x = fread(chunk, 1, chunkSize, dPtr);
            if(x <= 0){
                eof = true;
                continue;
            }
            cout<<i++<<"th. chunk of source file is: "<<chunk<<endl;
            index = checksumMatch(indexFile, chunk);
            if(index != -1){
                cout<<"--------Checksum matched-----------"<<endl;
                string blockStr = "\n"+DELIM+to_string(index)+DELIM+"\n";
                // fputs((char *)blockStr.c_str(), uPtr);
                fwrite((char *)blockStr.c_str(), 1, blockStr.length(), uPtr);
            }else{
                cout<<"--------Weak checksum did not matched-----------"<<endl;
                // fputc(chunk[0], uPtr);
                if(chunk[0] == '\n'){
                    string str = "####";
                    fwrite((char *)str.c_str(), 1, 4, uPtr);
                }else{
                    fwrite(&chunk[0], 1, 1, uPtr);
                }
                // cout<<"value of position is: "<<(unsigned char *)&position<<endl;
                fsetpos(dPtr, &position);
                fseek(dPtr, 1, SEEK_CUR);
                // cout<<"head after reset: "<<ftell(dPtr)<<endl;
            }
            memset(chunk, 0, chunkSize+1);
            fgetpos(dPtr, &position);
        }
        //writing dummy character in updateIndex file so that while merging last character doesn't miss out
        fwrite(&dummy, 1, 1, uPtr);
        fclose(dPtr);
        fclose(uPtr);
        cout<<"Update file created successfully.."<<endl;
        return updateFile;
    }

    int updateDataBackupFile(string dataBackFile, string updateIndexFile, int chunkSize){
        FILE *dbPtr, *uiPtr, *newPtr;
        dbPtr = fopen((char *)dataBackFile.c_str(), "rb");
        if(dbPtr == NULL){
            cout<<"Unable to open data file.."<<endl;
            return -1;
        }
        uiPtr = fopen((char *)updateIndexFile.c_str(), "rb");
        if(uiPtr == NULL){
            cout<<"Unable to update index file.."<<endl;
            return -1;
        }
        string newdataFile = dataBackFile+".new";
        newPtr = fopen((char *)newdataFile.c_str(), "wb");
        if(newPtr == NULL){
            cout<<"Unable to create new data file.."<<endl;
            return -1;
        }
        char *line;
        size_t len = 0;
        char chunk[chunkSize+1] = {0};
        while(getline(&line, &len, uiPtr) != -1){
            string dataLine = line;
            cout<<"Current line is: "<<dataLine;
            if(dataLine == "" || dataLine == "\n"){
                // cout<<"Empty Line.."<<endl;
                continue;
            }
            int index = checkValidIndex(line, DELIM);
            if(index == -1){
                cout<<"Invalid index so writing current line.."<<endl;
                // fputs(line, newPtr);
                // string temp = line;
                // temp = temp.substr(0, temp.length()-1);
                // fwrite((char *)temp.c_str(), 1, temp.length(), newPtr);
                // fwrite(line, 1, strlen(line), newPtr);
                for(int i=0; i<strlen(line)-1; i++){
                    if(line[i] != '#'){
                        fwrite(&line[i], 1, 1, newPtr);
                    }else if(line[i] == '#'){
                        if(i+3 < strlen(line) && line[i+1] == '#' && line[i+2] == '#' && line[i+3] == '#'){
                            string str = "\n";
                            fwrite((char *)str.c_str(), 1, 1, newPtr);
                            i += 3;
                        }else{
                            fwrite(&line[i], 1, 1, newPtr);
                        }
                    }
                }
            }else{
                cout<<"Valid index thus writing indexed block in backup file and index is: "<<index<<endl;
                fseek(dbPtr, index*chunkSize, SEEK_SET);
                // fgets(chunk, chunkSize, dbPtr);
                int x = fread(chunk, 1, chunkSize, dbPtr);
                // fputs(chunk, newPtr);
                cout<<"No of bytes read are: "<<x<<endl;

                fwrite(chunk, 1, x, newPtr);
                memset(chunk, 0, chunkSize+1);
            }
            line = NULL;
        }
        fclose(dbPtr);
        fclose(uiPtr);
        fclose(newPtr);

        //Comment below lines if you want to see the difference between old backup file and newly created backup file
        int x = remove((char *)dataBackFile.c_str());
        if (x != 0){
            cout<<"Unable to delete old backup file.."<<endl;
            return -1;
        }
        cout<<"Old backup file deleted successfully.."<<endl;
        x = rename((char *)newdataFile.c_str(), (char *)dataBackFile.c_str());
        if(x != 0){
            cout<<"Unable to rename newly created file as old backup file.."<<endl;
            return -1;
        }
        cout<<"New file is renamed as old backup file successfully.."<<endl;
        return 0;
    }

};
