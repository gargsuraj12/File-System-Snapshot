#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

//int mkdir(const char *pathname, mode_t mode);



void createDirectory(string path){
	int i=0;
	//char c[PATHMAX],t;
	 int n = path.length(); 
     
    // declaring character array
    char pathname[n+1]; 
     
    // copying the contents of the 
    // string to char array
    strcpy(pathname, path.c_str());

	if (mkdir(pathname,0777) == -1) {
        //perror(argv[0]);
        exit(EXIT_FAILURE);
    }
	
}


int main(){
	string str;
	getline(cin,str);
	createDirectory(str);
	return 0;
}
/*
int main(){
	int i=0;
	char c[1024],t;
	while(EOF!=(t=getc(stdin)) && t != '\n'){
		c[i] = t;
		i++;
	}
	if (mkdir(c,0777) == -1) {
        //perror(argv[0]);
        exit(EXIT_FAILURE);
    }

	return 0;	
}*/
