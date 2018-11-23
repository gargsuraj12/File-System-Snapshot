// #include<stdio.h>
// #include<iostream>
// #include<stdlib.h>
// #include <fcntl.h> // for open()
// #include<pwd.h>
// #include <vector>
// #include <time.h>
// #include <fstream>
// #include<string.h>
// #include<unistd.h> //for getcwd()
// #include<dirent.h> // opendir() 
// #include <sys/stat.h> // for stat

// #include "library.h"
// #include "HelperClass.h"


using namespace std;
char temp[512];
char inicwd[256];
void printcwd(){
    char cwd[256];
    getcwd(cwd,sizeof(cwd));
    //printf("%s\n",cwd);

}
void copy_file(char * source , char * destination,char * filename){
    char savecurdir[256];
    getcwd(savecurdir,sizeof(savecurdir));
    cout<<"received fname "<<filename<<endl;
    cout<<"and its loc is "<<source<<endl;
    chdir(source);
    cout<<"and current loc is "<<savecurdir<<endl;
    int fd = open(filename, O_RDONLY);
	printf("file descriptor fd = %d\n",fd);
	char buf[17];
	cout<<"doin chdir to "<<destination<<endl;
	int chdirval= chdir(destination);
	
	////printf("chdir returned val is %d\n",chdirval);
	
	

	int fd_for_w = open(filename, O_RDWR | O_CREAT , 0777);
	////printf("file descriptor fd_for_w = %d\n",fd_for_w);
	if(fd_for_w == -1){
		printf ("error number %d\n", errno);
		perror("program");
	}
	////printf("file opened \n");
	int sz1,sz;
	sz = read(fd,buf,16);
	while(sz>0){
	//////printf("called read(%d ,buf,10 ) returned %d bytes were read\n",fd,sz);
	buf[16]='\0';
	//////printf("%s ",buf);
	sz = read(fd,buf,16);
	sz1 = write(fd_for_w, buf,strlen(buf));
	}
	//file close
	/*file close*/
    chdir(source);
	int close_id = close (fd);
	if(close_id < 0){
		printf("erroe while closeing file 1\n");
		perror("c1");
		//exit(1);
	}
	////printf("file clsoed");
    chdir(destination);
	close_id = close (fd_for_w);
	if(close_id < 0){
		printf("erroe while closeing\n");
		perror("c1");
		//exit(1);
	}
	chdir(savecurdir);
	////printf("file clsoed");
}
void copy(char * source , char * destination,int isFirst){
    cout << "Incoming" << endl;
    if(isFirst){
        strcpy(temp,"");
    }
    int chdirval = chdir(source);
    if(chdir(source)){
        perror("invalid path");
        return;
    }
    //printcwd();
    struct stat info;
    char path[256];
    char dpath[256];
    DIR *dir = opendir(source);
    struct dirent * e;
    strcpy(path,source);
    strcat(path,"/");
    strcpy(dpath,destination);
    strcat(dpath,"/");
    while((e = readdir(dir))!=NULL){
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, ".."))
            continue;
        //printf("%s\n", e->d_name);
        strcat(path,e->d_name);
        strcat(dpath,e->d_name);
        //cout<<"path  is "<<path<<endl;
        //strcat(path,"/");
        //printf("path is source %s\n",path);
        //printf("ret val %d \n",stat(path,&info));
        if(!stat(path,&info)){
            if(S_ISDIR(info.st_mode))  // return 0 on success
            {   
                // strcpy(temp,e->d_name);
                // strcat(source,"/");
                // strcat(source,temp);
                // printf("new is source %s\n",source);
                cout<<"path for destination directory is "<<dpath<<endl;
                if (mkdir(dpath,0777) == -1)
                    perror("cant do mkdir");
                cout<<"path for directory is "<<path<<endl;
                copy(path , dpath , 0);
                //printf("is_dir directory is %s\n",e->d_name);

            }
            else if(S_ISREG(info.st_mode)){
                copy_file(source,destination,e->d_name);
                cout<<"path for file is "<<path<<endl;
                //printf("is_reg file is %s\n",e->d_name);
            }
            cout<<endl;
            
        }
        strcpy(path,source);
        strcat(path,"/");
        strcpy(dpath,destination);
        strcat(dpath,"/");
        //printf("%s\n", e->d_name);
    }
}

// int main(){
//     char source[256];
//     char destination[256];
//     cin>>source;
//     cin>>destination;
//     char temp[512];
//     getcwd(inicwd,sizeof(inicwd));
//     copy(source,destination,1);
// }