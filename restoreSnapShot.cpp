#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h> // for open()
#include <pwd.h>
#include <vector>
#include <time.h>
#include <fstream>
#include <string.h>
#include <unistd.h> //for getcwd()
#include <dirent.h> // opendir() 
#include <sys/stat.h> // for stat
#include <thread>
#include <pthread.h>  
#include <bits/stdc++.h>
#include <sstream>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include "compareSnapshot.h"

#define MDPath "./MetaDataFile/All_snapshot_details.txt"

using namespace std;

class RestoreSnapShot{

public:

	vector<string> split(char *phrase, string delimiter){
	    vector<string> list;
	    string s = string(phrase);
	    size_t pos = 0;
	    string token;
	    while ((pos = s.find(delimiter)) != string::npos) {
	        token = s.substr(0, pos);
	        list.push_back(token);
	        s.erase(0, pos + delimiter.length());
	    }
	    list.push_back(s);
	    return list;
	}


	void processRestoreSnapShot(string sourcePath,string destinationPath){

		SyncData syncDataObj;
		string line;
		char cwd[PATH_MAX];
		if (getcwd(cwd, sizeof(cwd)) != NULL) 
		{
			printf("Current working dir: %s\n", cwd);
		}

		syncDataObj.createSnapshotFunctionality(toCharArrayFromString(sourcePath),toCharArrayFromString(sourcePath));
		vector<string> fileLines;

		string sPath = sourcePath+"/.snapshot";

		ifstream snapshot(sPath);
		if(snapshot.is_open()){

			while(getline(snapshot,line)){
				fileLines.push_back(line);	
			}
		}

		fileLines[0] = destinationPath;

		snapshot.close();

		ofstream writeSnapshot;
		writeSnapshot.open(sPath);
		for(int i=0;i<fileLines.size();i++){
			
			writeSnapshot<<fileLines[i];

		}
		writeSnapshot.close();


		syncDataObj.createSnapshotFunctionality(toCharArrayFromString(destinationPath),toCharArrayFromString(destinationPath));

		syncDataObj.createSnapshotFunctionality(toCharArrayFromString(sourcePath),toCharArrayFromString(destinationPath));

		int status = remove((char *)sPath.c_str());
		if(status == -1){
			cout<<"Error while deleting .snapshot file in:  "<<sPath<<endl;
		}

		chdir(cwd);

		vector<compareSnapshot> diffList;


		if(getcwd(cwd, sizeof(cwd)) != NULL) 
		{
			printf("Current working dir: %s\n", cwd);
		}

		diffList = syncDataObj.compareSnapshotFile(destinationPath,sourcePath);

	    syncDataObj.runTasks(diffList);

	}


	string getSnapShotLoaction(string source){

		ifstream metadata(MDPath);
		
		string destination = "";
		string line;

		if(metadata.is_open()){

			while(getline(metadata,line)){

				char *fileLine = new char [line.length()+1];	
				strcpy(fileLine, line.c_str());				

				vector<string> tokens = split(fileLine,"\t");

				if(tokens[0] == source){
					destination = tokens[1];
					metadata.close();
					return destination;
				}

			}

		}else{

			cout<<"Error opening metadata file\n";

		}

		metadata.close();

		return destination;
	}

	void restore(string source){

		string destination = getSnapShotLoaction(source);

		if(destination!=""){
		
			processRestoreSnapShot(source,destination);	
		
		}else{

			cout<<"No snapshot found\n";
		}
	}
};



int main(){

	string source;
	cout<<"enter source path\n";
	cin>>source;

	RestoreSnapShot restoreSnapShotObj;
	restoreSnapShotObj.restore(source); 

	return 0;
}