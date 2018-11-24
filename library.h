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

#define MDPath "./MetaDataFile/All_snapshot_details.txt"
//#define timeInterval 1200000 //20 minutes
#define timeInterval 15 //5sec

using namespace std;



struct snapshotDetails{

	string fullQualifiedPath;
	string ownership;
	string timeStamp;
	string accessRights;
	bool isFile=false;          // 1 -> file else directory
};

struct compareSnapshot{

	struct snapshotDetails details;
	string newTimeStamp;
	string oldTimeStamp;
	string operationType;
};

struct SnapShotMetaDataInformation
{
	string sourcePath;
	string destinationPath;
	string creationTimeStamp;
	string lastRunTime;
};