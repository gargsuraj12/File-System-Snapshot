#ifndef CREATEDIR_H
#define CREATEDIR_H

using namespace std;


void createDirectory(string path){
	int i=0;
	int n = path.length(); 
     
    char pathname[n+1]; 
     
    strcpy(pathname, path.c_str());

	if (mkdir(pathname,0777) == -1) {
        exit(EXIT_FAILURE);
    }
	
}

#endif