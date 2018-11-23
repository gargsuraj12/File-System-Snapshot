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

#define MODVAL 1048576
static const int CHUNKSIZE  = 524288;
static const string DELIM = "$";
// static const string END_DELIM = "$|";

struct checksumValues{
    int r1,r2;
    string checksum;
};

//Splits the string on the basis of the given character
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
    // cout<<"Delimeter is: "<<delimeter<<endl;
    vector<string> vec = splitLine(line, delimeter);
    // cout<<"vector size after splitting is: "<<vec.size()<<endl;
    for(auto it : vec){
        cout<<it<<endl;
    }
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
    // cout<<"Chunk for MD5 is: "<<chunk<<" and len of chunk is: "<<strlen(chunk)<<endl;
    // unsigned char digest[MD5_DIGEST_LENGTH];
    // string checksum;
    // MD5((unsigned char *)chunk, strlen(chunk), (unsigned char *)&digest);
    // if(strlen((char *)digest) == 0){
	// 	cout<<"Unable to calculate MD5 hash.."<<endl;
	// 	return "";
	// }
    // char mdString[MD5_DIGEST_LENGTH*2+1];
    // for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
    //     sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
	// }
    // checksum = mdString;
    // return checksum;
    // SHA_CTX mdContent;
    unsigned char digest[SHA_DIGEST_LENGTH];
	string retHash;
    // SHA1_Init(&mdContent);
	SHA1((unsigned char*)chunk, strlen(chunk), (unsigned char*)&digest);
	if(strlen((char *)digest) == 0){
		cout<<"Unable to calculate hash"<<endl;
		return "";
	}
   char mdString[SHA_DIGEST_LENGTH*2+1];
	// cnt++;
   for(int i = 0; i < SHA_DIGEST_LENGTH; i++){
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
	}
	retHash = mdString;
	//cout<<"Hash for "<<cnt<<" is:"<<retHash<<endl;
	return retHash;
}

//calculates weak checksum for the given data chunk and return values of a, b and checksum
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

//calculate increamental rolling checksum using previous checksum
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

int prepareIndexOfBackupFile(string dataFilename, int chunkSize){
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
    char chunk[chunkSize+1]= {0};
    string weakChecksum, strongChecksum;
    checksumValues cv;
    int j=0;
    char ch;
    bool eof = false;
    while(!eof){//fgets(chunk, chunkSize, dataFile)
        // int i=0;
        // while((ch=getc(dataFile)) != EOF){
        //     chunk[i] = ch;
        //     i++;
        //     if(i == chunkSize){
        //         break;
        //     }
        // }
        int x = fread(chunk, 1, chunkSize, dataFile);
        if(x<=0){   //ch == EOF
            eof = true;
            continue;
        }
        cout<< j++ <<". Chunk in createIndexFile() is: "<<chunk<<endl;
        cv = calcRollingChecksum(chunk);
        strongChecksum = calcMD5(chunk);
        if(strongChecksum == ""){
            fclose(dataFile);
            fclose(indexFile);
            return -1;
        }
        fprintf(indexFile, "%s:%s:%d\n",(char *)cv.checksum.c_str(), (char *)strongChecksum.c_str(), index);
        index++;
        memset(chunk, 0, chunkSize+1);
        // if(ch == EOF){   //
        //     eof = true;
        // }
    }
    fclose(dataFile);
    fclose(indexFile);
    return 0;
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
    while(getline(&line, &len, iPtr) != -1){
        vector<string> vec = splitLine(line, ":");
        weakChecksum = calcRollingChecksum(chunk).checksum;

        if(weakChecksum == vec[0]){
            // cout<<"Weak checksum matched which is: "<<weakChecksum<<endl;
            strongChecksum = calcMD5((char *)chunk.c_str());
            if(strongChecksum == vec[1]){
                indexStr = vec[2].substr(0, vec[2].length()-1);
                index = stoi((char *)indexStr.c_str());
                return index;
            }
        }
    }
    fclose(iPtr);
    return -1;
}

int prepareUpdateIndexFile(string srcDataFile, string indexFile, int chunkSize){
    FILE *dPtr, *uPtr;
    dPtr = fopen((char *)srcDataFile.c_str(), "rb");
    if(dPtr == NULL){
        cout<<"Unable to open data file.."<<endl;
        return -1;
    }
    string updateFile = srcDataFile+".updateIndex";
    uPtr = fopen((char *)updateFile.c_str(), "wb");
    if(uPtr == NULL){
        cout<<"Unable to create update file.."<<endl;
        return -1;
    }
    char chunk[chunkSize+1] = {0};
    string weakChecksum, strongChecksum;
    int index;
    fpos_t position;
    fsetpos(dPtr, &position);
    int i=0;
    bool eof = false;
    char ch;
    // cout<<i++<<". Chunk in prepareUpdateIndexFile() is: "<<chunk<<endl;
    while(!eof){
        // int j=0;
        // while((ch=getc(dPtr)) != EOF){
        //     chunk[j] = ch;
        //     j++;
        //     if(j == chunkSize){
        //         break;
        //     }
        // }
        int x = fread(chunk, 1, chunkSize, dPtr);
        if(x <= 0){
            eof = true;
            continue;
        }
        cout<<i++<<". Chunk in prepareUpdateIndexFile() is: "<<chunk<<endl;
        index = checksumMatch(indexFile, chunk);
        if(index != -1){
            cout<<"--------Checksum matched-----------"<<endl;
            string blockStr = "\n"+DELIM+to_string(index)+DELIM+"\n";
            fputs((char *)blockStr.c_str(), uPtr);
        }else{
            cout<<"--------Weak checksum did not matched-----------"<<endl;
            fputc(chunk[0], uPtr);
            fsetpos(dPtr, &position);
            fseek(dPtr, 1, SEEK_CUR);
            // cout<<ftell(dPtr)<<endl;
        }
        memset(chunk, 0, chunkSize+1);
        fgetpos(dPtr, &position);
        // if(ch == EOF){
        //     eof = true;
        // }
    }
    fclose(dPtr);
    fclose(uPtr);
    cout<<"Update file created successfully.."<<endl;
    return 0;
}

int prepareUpdateIndexFile_Backup(string srcDataFile, string indexFile, int chunkSize){
    FILE *dPtr, *uPtr;
    dPtr = fopen((char *)srcDataFile.c_str(), "rb");
    if(dPtr == NULL){
        cout<<"Unable to open data file.."<<endl;
        return -1;
    }
    string updateFile = srcDataFile+".updateIndex";
    uPtr = fopen((char *)updateFile.c_str(), "wb");
    if(uPtr == NULL){
        cout<<"Unable to create update file.."<<endl;
        return -1;
    }
    char chunk[chunkSize+1] = {0};
    string weakChecksum, strongChecksum;
    int index;
    fpos_t position;
    fsetpos(dPtr, &position);
    char *status = fgets(chunk, chunkSize, dPtr);
    cout<<"Staus is: "<<status<<endl;
    int i=0;
    bool eof = false;
    // cout<<i++<<". Chunk in prepareUpdateIndexFile() is: "<<chunk<<endl;
    while(strlen(chunk) != 0){
        cout<<i++<<". Chunk in prepareUpdateIndexFile() is: "<<chunk<<endl;
        index = checksumMatch(indexFile, chunk);
        if(index != -1){
            cout<<"--------Checksum matched-----------"<<endl;
            string blockStr = "\n"+DELIM+to_string(index)+DELIM+"\n";
            fputs((char *)blockStr.c_str(), uPtr);
        }else{
            cout<<"--------Weak checksum did not matched-----------"<<endl;
            fputc(chunk[0], uPtr);
            fsetpos(dPtr, &position);
            fseek(dPtr, 1, SEEK_CUR);
            // cout<<ftell(dPtr)<<endl;
        }
        memset(chunk, 0, chunkSize+1);
        fgetpos(dPtr, &position);
        if(!eof && !fgets(chunk, chunkSize, dPtr)){
            eof = true;
        }
    }
    fclose(dPtr);
    fclose(uPtr);
    cout<<"Update file created successfully.."<<endl;
    return 0;
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
            fwrite(line, 1, strlen(line), newPtr);
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
    }
    fclose(dbPtr);
    fclose(uiPtr);
    fclose(newPtr);

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


int main(){

    // prepareIndexOfBackupFile("Id.jpg", CHUNKSIZE);
    // prepareUpdateIndexFile("updated.jpg", "Id.jpg.index", CHUNKSIZE);

    int x = updateDataBackupFile("Id.jpg", "updated.jpg.updateIndex", CHUNKSIZE);
    if(x != 0){
        cout<<"Error occured while updating the backup file.."<<endl;
    }else{
        cout<<"Backup file updated successfully.."<<endl;
    }

    return 0;
}