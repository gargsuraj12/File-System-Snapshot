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
#define MDLogFilePath "./LogFile/logfile.txt"


using namespace std;

class RestoreSnapShot{

public:

	bool toLog = true;

	char * getCurrentTime()
	{
	    std::time_t result = std::time(nullptr);
	    return std::asctime(std::localtime(&result));
	}

	bool writeLog(string Data,int flag)
	{
		if(toLog==0)
			return false;

		bool success = false;
		if(Data!="")
		{
			std::ofstream out;
	 		out.open(MDLogFilePath, std::ios::app);
	 		string strDate(getCurrentTime());
	 		if(flag == 1)
	 		{
	 			out << "Information : " ;
	 		}
	 		if(flag == -1)
	 		{
	 			out << "Error : " ;
	 		}
	 		out <<  strDate << ":" << Data << endl;
			out.close();
		}
		return success;
	}

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
			
			writeSnapshot<<fileLines[i]<<endl;

		}
		writeSnapshot.close();


		// syncDataObj.createSnapshotFunctionality(toCharArrayFromString(destinationPath),toCharArrayFromString(destinationPath));

		// syncDataObj.createSnapshotFunctionality(toCharArrayFromString(sourcePath),toCharArrayFromString(destinationPath));

		// int status = remove((char *)sPath.c_str());
		// if(status == -1){
		// 	cout<<"Error while deleting .snapshot file in:  "<<sPath<<endl;
		// }

		chdir(cwd);

		vector<compareSnapshot> diffList;


		// if(getcwd(cwd, sizeof(cwd)) != NULL) 
		// {
		// 	printf("Current working dir: %s\n", cwd);
		// }

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

	bool restoreSnapShotInDestination(string strFolderPath){

		remove(toCharArrayFromString(toCharArrayFromString(strFolderPath+"/.snapshot")));
		// rename(toCharArrayFromString(strFolderPath+"/__tempBuffer_2.txt"),toCharArrayFromString(strFolderPath+"/.snapshot"));
		rename(toCharArrayFromString(strFolderPath+"/__tempBuffer_2.txt"),toCharArrayFromString(strFolderPath+"/.snapshot"));

		return true;
	}

	void restore(string source){

		string destination = getSnapShotLoaction(source);
		cout<<"destination "<<destination<<"\n";
		if(!CreateTemporaryFileForSnapShot(destination)){
			writeLog("Swapping .snapshot file ",-1);
		}

		if(destination!=""){
		
			processRestoreSnapShot(source,destination);	
		
		}else{

			cout<<"No snapshot found\n";
		}

		//restoring .snapshot file in destination folder

		if(remove(toCharArrayFromString(source+"/.snapshot"))!=0){
			writeLog("Source snapshot file remove error",-1);
		}

		if(!restoreSnapShotInDestination(destination)){
			writeLog("Re-swapping .snapshot file ",-1);	
		}

	}


	//strPath - snapshot file path
	// strFolderPath - snapshot file folder Path 
	bool CreateTemporaryFileForSnapShot(string strFolderPath)
	{
		string deleteline;
		std::string line;
		
		string strPath = strFolderPath+"/.snapshot";

		std::ifstream file(strPath);
		ofstream temp;
		temp.open(strFolderPath+"/__tempBuffer.txt");

		ofstream temp2;
		temp2.open(strFolderPath+"/__tempBuffer_2.txt");


		vector<string> tokens;
		bool firstLine=false;
		
		vector<string> fileContent;

		while (getline(file,line))
		{
			fileContent.push_back(line);
			temp2 << line << endl;
		}

		string tempLine = fileContent[0];
		fileContent[0]=fileContent[1];
		fileContent[1]=tempLine;

		for(auto lineContent : fileContent)
		{
			temp << lineContent << endl;
		}

		temp.close();
		temp2.close();
		file.close();

		char * strPath1; 

		strPath1 = (char*)malloc(strFolderPath.length()+1);

    		strcpy(strPath1,strFolderPath.c_str());
    	
    	remove(toCharArrayFromString(toCharArrayFromString(strFolderPath+"/.snapshot")));
		// rename(toCharArrayFromString(strFolderPath+"/__tempBuffer_2.txt"),toCharArrayFromString(strFolderPath+"/.snapshot"));
		rename(toCharArrayFromString(strFolderPath+"/__tempBuffer.txt"),toCharArrayFromString(strFolderPath+"/.snapshot"));
		
		return true;
	}

	char* toCharArrayFromString(string str)
	{
	    // char charStr[str.length()+1];
	    char *charStr;

	    charStr = (char*)malloc(str.length()+1);

	    strcpy(charStr,str.c_str());
	    return charStr;
	}

};



int main(int argc,char *argv[]){

	// string source="/home/prakashjha/os/workarea/OS_Snapshot/repoSnapShot";
	
	RestoreSnapShot restoreSnapShotObj;	

	restoreSnapShotObj.writeLog("---Entering into restoreSnapshot main function",1);

	// string source;
	// cout<<"enter source path\n";
	// cin>>source;
	
	if(argc<2){
		restoreSnapShotObj.writeLog("Invalid input format",-1);
		return -1;
	}

	restoreSnapShotObj.writeLog(to_string(argc),1);
	restoreSnapShotObj.writeLog(argv[1],1);

	string source(argv[1]);

	//Step 1 :Fetch Destination Path From Metadata 
	// Step 2: Make SnapShot in reverse Order
	//Step 3 : Call Compare SnapShot and runTask 
	// RestoreSnapShot restoreSnapShotObj;
	restoreSnapShotObj.restore(source); 

	restoreSnapShotObj.writeLog("---Exiting from restoreSnapshot main function",1);

	return 0;
}