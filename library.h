#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include <fcntl.h> // for open()
#include<pwd.h>
#include <vector>
#include <time.h>
#include <fstream>
#include<string.h>
#include<unistd.h> //for getcwd()
#include<dirent.h> // opendir() 
#include <sys/stat.h> // for stat

#include<bits/stdc++.h>

using namespace std;

struct snapshotDetails{

	string fullQualifiedPath;
	string ownership;
	string timeStamp;
	string accessRights;
};

struct compareSnapshot{

	struct snapshotDetails details;
	string newTimeStamp;
	string oldTimeStamp;
	string operationType;

};