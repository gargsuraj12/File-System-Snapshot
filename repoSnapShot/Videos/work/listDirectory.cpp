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
#include <linux/kernel.h>
#include <linux/fs.h>
#include <bits/stdc++.h>

char pathname[MAXPATHLEN];


using namespace std;


void printdir(vector<pair<string, int>> directory){
for(int i=0;i<directory.size();i++){
        
        cout<<directory[i].first<<" "<<directory[i].second<<"\n";
    }
}


static char perms_buff[30];


const char *get_perms(mode_t mode)
{
  char ftype = '?';

  if (S_ISREG(mode)) ftype = '-';
  if (S_ISLNK(mode)) ftype = 'l';
  if (S_ISDIR(mode)) ftype = 'd';
  if (S_ISBLK(mode)) ftype = 'b';
  if (S_ISCHR(mode)) ftype = 'c';
  if (S_ISFIFO(mode)) ftype = '|';

  sprintf(perms_buff, "%c%c%c%c%c%c%c%c%c%c %c%c%c", ftype,
  mode & S_IRUSR ? 'r' : '-',
  mode & S_IWUSR ? 'w' : '-',
  mode & S_IXUSR ? 'x' : '-',
  mode & S_IRGRP ? 'r' : '-',
  mode & S_IWGRP ? 'w' : '-',
  mode & S_IXGRP ? 'x' : '-',
  mode & S_IROTH ? 'r' : '-',
  mode & S_IWOTH ? 'w' : '-',
  mode & S_IXOTH ? 'x' : '-',
  mode & S_ISUID ? 'U' : '-',
  mode & S_ISGID ? 'G' : '-',
  mode & S_ISVTX ? 'S' : '-');

  return (const char *)perms_buff;
}


void die(char *msg)
{
  perror(msg);
  exit(0);
}

static int
one (const struct dirent *unused)
{
  return 1;
}


void listDirectories(char *basePath){

  int count,i;
  struct dirent **files;
  struct stat statbuf;
  char datestring[256];
  struct passwd pwent;
  struct passwd *pwentp;
  struct group grp;
  struct group *grpt;
  struct tm time;
  char buf[1024];
  int k=0;

  count = scandir(basePath, &files, one, alphasort);

  if(count > 0)
  {
    //printf("total %d\n",count);
    
    for (i=2; i<count; i++)
    {
      //if (stat(files[i]->d_name, &statbuf) == 0)
      //{
        /* Print out type, permissions, and number of links. */
        //string status = get_perms(statbuf.st_mode);
        stat(files[i]->d_name, &statbuf);
        printf("%10.10s", get_perms(statbuf.st_mode));
        printf(" %d", statbuf.st_nlink);

        if (!getpwuid_r(statbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))
          printf(" %s", pwent.pw_name);
        else
          printf(" %d", statbuf.st_uid);

        if (!getgrgid_r (statbuf.st_gid, &grp, buf, sizeof(buf), &grpt))
          printf(" %s", grp.gr_name);
        else
          printf(" %d", statbuf.st_gid);

        /* Print size of file. */
        printf(" %5d", (int)statbuf.st_size);

        localtime_r(&statbuf.st_mtime, &time);
        /* Get localized date string. */
        strftime(datestring, sizeof(datestring), "%F %T", &time);

        printf(" %s %s\n", datestring, files[i]->d_name);

    //  }
     
    }
  }
}


void listFilesRecursively(char *basePath,vector<pair<string, int>> &directory)
{
    cout<<"dfgdfgdf\n";
    char path[PATH_MAX];
    string temp;
    int flag=1;
    struct dirent *dp;
    struct stat statbuf;
    char datestring[256];
    struct passwd pwent;
    struct passwd *pwentp;
    struct group grp;
    struct group *grpt;
    struct tm time;
    char buf[1024];

    DIR *dir = opendir(basePath);
    if(dir){
        temp = string(basePath);
        //directory.push_back(temp);
        directory.push_back(pair<string, int> (temp,1));

        while ((dp = readdir(dir)) != NULL)
        {
            string str;
            if (stat(dp->d_name, &statbuf) == 0)
            {
                /* Print out type, permissions, and number of links. */
                //string status = get_perms(statbuf.st_mode);
                printf("%10.10s", get_perms(statbuf.st_mode));
                printf(" %d", statbuf.st_nlink);

                if (!getpwuid_r(statbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))
                    printf(" %s", pwent.pw_name);
                else
                    printf(" %d", statbuf.st_uid);

                if (!getgrgid_r (statbuf.st_gid, &grp, buf, sizeof(buf), &grpt))
                    printf(" %s", grp.gr_name);
                else
                    printf(" %d", statbuf.st_gid);

                /* Print size of file. */
                printf(" %5d", (int)statbuf.st_size);

                localtime_r(&statbuf.st_mtime, &time);
                /* Get localized date string. */
                strftime(datestring, sizeof(datestring), "%F %T", &time);

                printf(" %s %s\n", datestring, dp->d_name);

            }
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
            {
                strcpy(path, basePath);
                strcat(path, "/");
                strcpy(path, dp->d_name);
                //printf("%s\n",path );
                str = string(path);
                directory.push_back(pair<string, int> (str,0));
            }
        }
        closedir(dir);
    }
}


int main(int argc, char *argv[])
{
    // Directory path to list files
    char path[PATH_MAX];
    vector<pair<string,int>> directory;

    //getcwd(path, sizeof(pathname));

    //cout<<string(path)<<"\n";
    //directory.push_back(pair<string, int> (string(argv[1]),1));    
    
    //listDirectories(argv[1]);
    listFilesRecursively(argv[1],directory);
    
    /*
    int i=1;
    while(i<directory.size()){

        strcpy(path, (directory[i].first).c_str());
        //cout<<string(path)<<"\n";
        if(directory[i].second==0)
        listDirectories(path,directory);
        i++;
    }
    */
    printdir(directory);
    
    return 0;
}


