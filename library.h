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


#define MDPath "./MetaDataFile/All_snapshot_details.txt"
#define MDPathLogFile "./LogFile/logfile.txt"
//#define timeInterval 1200000 //20 minutes
#define timeInterval 15 //5sec
#define MODVAL 1048576
// static const string DELIM = "$";

using namespace std;

// struct checksumValues{
//     int r1,r2;
//     string checksum;
// };

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

//redefined in Main.cpp
struct SnapShotMetaDataInformation
{
	string sourcePath;
	string destinationPath;
	string creationTimeStamp;
	string lastRunTime;
};