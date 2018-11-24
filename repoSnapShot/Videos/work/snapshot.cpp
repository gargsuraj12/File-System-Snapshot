#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <iomanip>
#include <ctime>

char pathname[MAXPATHLEN];


using namespace std;


void printdir(vector<pair<string, int>> directory){
	for(int i=0;i<directory.size();i++)
		if(directory[i].second==1)
			cout<<"\n\nDirectory: "<<directory[i].first<<directory[i].second<<"\n\n";
		else
			cout<<directory[i].first<<directory[i].second<<"\n";		
}

void createDump(vector<pair<string, int>> directory,string destination){

	ofstream myfile;
	//string str = "snapshot_dump.txt";

	myfile.open (destination);

	for(int i=0;i<directory.size();i++){
  		//myfile << "Writing this to a file.\n";
	
 		if(directory[i].second==1){
 			myfile<<"\n\nDirectory: "<<directory[i].first<<"\n\n";
 		}
		else{
			
			size_t found = directory[i].first.find_last_of("/\\");
		    myfile<<directory[i].first.substr(found+1)<<"\n";
		}
			
		
	}	
	myfile.close();
  
}

/**
 * Lists all files and sub-directories recursively 
 * considering path as base path.
 */

void listFilesRecursively(char *basePath,vector<pair<string, int>> &directory)
{
    char path[PATH_MAX];
    string temp;
    int flag=1;
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    if(dir){
    	temp = string(basePath);
    	//directory.push_back(temp);
    	directory.push_back(pair<string, int> (temp,1));

    while ((dp = readdir(dir)) != NULL)
    {
    	string str;

    	if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            //strcpy(path, basePath);
            //strcat(path, "/");
            strcpy(path, dp->d_name);
            //printf("%s\n",path );
            str = temp+"/"+string(path);
    		directory.push_back(pair<string, int> (str,0));
    	}
    }
    	closedir(dir);
	}
}


int main(int argc, char const *argv[])
{
    // Directory path to list files
    //cout<<"\nits fired\n";
    char path[PATH_MAX];
    char destinationPath[PATH_MAX];
    
    vector<pair<string,int>> directory;

    //getcwd(path, sizeof(pathname));

    //cout<<string(path)<<"\n";

    strcpy(path, argv[1]);

    strcpy(destinationPath, argv[2]);

    string dest = string(destinationPath);

    //printf("%s\n", path);

    listFilesRecursively(path,directory);
    int i=1;
    while(i<directory.size()){

    	strcpy(path, (directory[i].first).c_str());
    	//cout<<string(path)<<"\n";
    	if(directory[i].second==0)
    	listFilesRecursively(path,directory);
    	i++;
    }

    //printdir(directory);
    createDump(directory,dest);
    return 0;
}


