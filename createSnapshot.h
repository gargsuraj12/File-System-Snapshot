// #include<stdio.h>
// #include<iostream>
// #include<stdlib.h>
// #include<pwd.h>
// #include <vector>
// #include <time.h>
// #include <fstream>
// #include<string.h>
// #include<unistd.h> //for getcwd()
// #include<dirent.h> // opendir() 
// #include <sys/stat.h> // for stat
using namespace std;

class CreateSnapShotClass
{
    
public : 

char temp[512];
vector<string> entries;
vector<string> owner;
vector<string> accesstime;
vector<string> isfile;
vector<string> accessrights;
char inicwd[256];

void printcwd(){
    char cwd[256];
    getcwd(cwd,sizeof(cwd));
    //printf("%s\n",cwd);
}

void prepareMetadataForSnapshot(char * source , char * destination,int isFirst){
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
    DIR *dir = opendir(source);
    struct dirent * e;
    strcpy(path,source);
    strcat(path,"/");
    while((e = readdir(dir))!=NULL){
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, ".."))
            continue;
        //printf("%s\n", e->d_name);
        strcat(path,e->d_name);
        entries.push_back((path));
        
        //strcat(path,"/");
        //printf("path is source %s\n",path);
        //printf("ret val %d \n",stat(path,&info));
        if(!stat(path,&info)){
            struct passwd *pw = getpwuid(info.st_uid);
            accesstime.push_back(ctime(&info.st_mtime));
            //printf("Last modified time: %s", ctime(&info.st_mtime));
            owner.push_back(pw->pw_name);
            char *modeval = (char *)malloc(sizeof(char) * 9 + 1);
            
            mode_t perm = info.st_mode;
            modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
            modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
            modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
            modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
            modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
            modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
            modeval[6] = (perm & S_IROTH) ? 'r' : '-';
            modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
            modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
            modeval[9] = '\0';
            //cout<< modeval;     
            
            accessrights.push_back(modeval);


            if(S_ISDIR(info.st_mode))  // return 0 on success
            {   
                // strcpy(temp,e->d_name);
                // strcat(source,"/");
                // strcat(source,temp);
                // printf("new is source %s\n",source);
                isfile.push_back("dir");
                prepareMetadataForSnapshot(path , destination , 0);
                //printf("is_dir directory is %s\n",e->d_name);

            }
            else if(S_ISREG(info.st_mode)){
            	isfile.push_back("file");
                //printf("is_reg file is %s\n",e->d_name);
            }
            
        }
        strcpy(path,source);
        strcat(path,"/");
        //printf("%s\n", e->d_name);
    }
}

void CreateSnapshotFile(char * source,char * destination ){
    //FILE *fp;
    //fp =fopen("snapshot1.txt","w");
    //if(!fp)
    //cout<<"casdsa ";
     //to be discussed with chitta

    /*
    int chdirval = chdir(inicwd);
    if(chdir(inicwd)){
        perror("invali$$$$$$$d path");
        return;
    }
    */
    string str(destination);
    string path = str + "/.snapshot";
    // ofstream fout("snapshot1.txt");
    ofstream fout(path);
    if (fout.is_open())
    {
    	time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fout<<source<<endl<<destination<<endl;
    fout<<"snapshot created "<<tm.tm_year + 1900<<"\t"<< tm.tm_mon + 1
    <<"\t"<< tm.tm_mday<<"\t"<< tm.tm_hour<<"\t"<< tm.tm_min<<"\t"<< tm.tm_sec<<endl;
    //fprintf(fp,"%s\n%s\n",source,destination );
    //fprintf(fp,"snapshot created: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    for(int i=0;i<entries.size();i++){
        //fprintf (fp , "%s\t%s\t%s\t%s" , entries[i],owner[i],accessrights[i],accesstime[i]);
        fout<<entries[i]<<"\t"<<isfile[i]<<"\t"<<owner[i]<<"\t"<<accessrights[i]<<"\t"<<accesstime[i];
    }
    //cout<<"casdsa asdasd";
        //cout<<entries[i]<<" "<<owner[i]<<" "<<accessrights[i]<<" "<<accesstime[i]<<endl;
    //fclose(fp);
    fout.close();
    }
    else
    {
        cout << "File Not Found" << endl;
    }
}

};

// int main(){
//     char source[256];
//     char destination[256];
//     cin>>source;
//     cin>>destination;
//     char temp[512];
//     getcwd(inicwd,sizeof(inicwd));
//     prepareMetadataForSnapshot(source,destination,1);
//     //cout<<"to write in file"<<endl<<endl;
//     CreateSnapshotFile(source,destination);
// }

