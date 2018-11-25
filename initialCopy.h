//#include "rsync.h"
//#include "library.h"

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
#include <string>

using namespace std;



#define MDPathLogFile "./LogFile/logfile.txt"

class CopyFunctionality
{
    public:
        char temp[512];
        char inicwd[256];

        char masterCopyLocation[512];

    bool writeLog(string Data)
    {
        bool success = false;
        if(Data!="")
        {
            ///home/prakashjha/os/workarea/OS_Snapshot/LogFile
            std::ofstream out;
            out.open(MDPathLogFile, std::ios::app);
            out << getCurrentTime() << ":" << Data << endl;
            out.close();
        }
        return success;
    }  
    
    char * getCurrentTime()
    {
        std::time_t result = std::time(nullptr);
        return std::asctime(std::localtime(&result));
    }


    int cp(const char *to, const char *from)
    {
        int fd_to, fd_from;
        char buf[4096];
        ssize_t nread;
        int saved_errno;

        fd_from = open(from, O_RDONLY);
        if (fd_from < 0)
            return -1;

        fd_to = open(to, O_WRONLY | O_CREAT , 0666);
        if (fd_to < 0)
            goto out_error;

        while (nread = read(fd_from, buf, sizeof buf), nread > 0)
        {
            char *out_ptr = buf;
            ssize_t nwritten;

            do {
                nwritten = write(fd_to, out_ptr, nread);

                if (nwritten >= 0)
                {
                    nread -= nwritten;
                    out_ptr += nwritten;
                }
                else if (errno != EINTR)
                {
                    goto out_error;
                }
            } while (nread > 0);
        }

        if (nread == 0)
        {
            if (close(fd_to) < 0)
            {
                fd_to = -1;
                goto out_error;
            }
            close(fd_from);

            /* Success! */
            return 0;
        }

    out_error:
        saved_errno = errno;

        close(fd_from);
        if (fd_to >= 0)
            close(fd_to);

        errno = saved_errno;
        return -1;
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
        writeLog("Start Of copy for source: "+(string)source +" and destination: "+(string)destination);
        cout<<"---Inside copy() function---"<<endl;
        if(getcwd(masterCopyLocation, sizeof(masterCopyLocation)) != NULL) {
            writeLog("Location Of File");
            writeLog(masterCopyLocation);
        }

        if(isFirst){
            strcpy(temp,"");
        }
        int chdirval = chdir(source);
        if(chdir(source)){
            char temppath[1024];
            struct stat infofile;
            strcpy(temppath,source);
            if(!stat(temppath,&infofile)){
                if(S_ISREG(infofile.st_mode)){
                    //copy_file(source,destination,e->d_name);

                    writeLog("Location Of File 2");
                    writeLog(masterCopyLocation);
                    
                    int lastindex = 0;
                    for(int i=0;i<strlen(temppath);i++){
                        if(temppath[i]=='/')
                            lastindex = i;
                    }
                    char filen[256];
                    char sd[1024];
                    int j=0;int k=0;
                    for(int i=0;i<strlen(temppath);i++){
                        if(i>lastindex){
                            filen[j]=temppath[i];
                            j++;
                        }
                        else{
                            sd[k]=temppath[i];
                            k++;
                        }
                    }

                    writeLog("Location Of File 3");
                    writeLog(masterCopyLocation);
                    

                    sd[k]='\0';
                    filen[j]='\0';
                    strcpy(sd,destination);
                    strcat(sd,"/");
                    strcat(sd,filen);
                    cout<<"filen is "<<filen<<" and is loc is "<<sd<<endl;
                    cout<<"Source is: "<<source<<endl;
                    cp(sd,source);
                   
                   // writeLog("Chunk size of file is: "+chunkSize);
                    writeLog("Location Of File XXXX");
                    writeLog(masterCopyLocation);
                     writeLog("Chunk size of file is: &&&&&&&&&&&&&&&&&&7");

                    string str1(sd);


                    // int chunkSize = ceil(sqrt(infofile.st_size));
                    // cout<<"Chunk size of file is: "<<chunkSize<<endl;
                    
                    writeLog("full path for rsync "+str1);
                    cout<<"full path for rsync "<<sd<<endl;
                    
                    // Rsync rObj;
                    // rObj.prepareIndexOfBackupFile(sd,chunkSize);
                    
                    //cout<<"filen is "<<filen<<" and is loc is "<<sd<<endl;
                    //copy_file(sd,destination,filen);
                    cout<<"path for file is "<<temppath<<endl;
                    //printf("is_reg file is %s\n",e->d_name);

                    writeLog("File copied successfully at "+(string)sd);
                }
            }
            else
                perror("invalid path");
            return;
        }
        //printcwd();
        //struct stat info;
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
                    if (mkdir(dpath,0777) == -1){
                        char errmsg[256];
                        strcpy(errmsg,"cant do mkdir for ");
                        strcat(errmsg,dpath);
                        perror(errmsg);
                    }
                    cout<<"path for directory is "<<path<<endl;
                    copy(path , dpath , 0);
                    //printf("is_dir directory is %s\n",e->d_name);

                }
                else if(S_ISREG(info.st_mode)){
                    //copy_file(source,destination,e->d_name);

                    writeLog("Location Of File Before copy ");
                    writeLog(masterCopyLocation);
                    
                    
                    cp(dpath,path);

                   // writeLog("Chunk size of file is: "+chunkSize);
                    writeLog("Location Of File YYYY");
                    writeLog(masterCopyLocation);
                     writeLog("Chunk size of file is: &&&&&&&&&&&&&&&&&&7");


                    // int chunkSize = ceil(sqrt(info.st_size));
                    // cout<<"Chunk size of file is: "<<chunkSize<<endl;
                    

                    string str1(dpath);

                    writeLog("full path for rsync "+str1);
                    //cout<<"full path for rsync "<<sd<<endl;
                    // Rsync rObj;
                    // rObj.prepareIndexOfBackupFile(dpath,chunkSize);
                    

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
};

// int main(){
//     char source[256];
//     char destination[256];
//     cin>>source;
//     cin>>destination;
//     CopyFunctionality obj;
    
//     char temp[512];
//     //getcwd(inicwd,sizeof(inicwd));
//     obj.copy(source,destination,1);
// }
