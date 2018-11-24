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

using namespace std;

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

char pathname[PATH_MAX];

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

struct directoryInfo {
    int count;
    char type;
    char dir_path[PATH_MAX];
    struct fileInfo *fileHead;
    struct directoryInfo *parent = NULL;
    struct directoryInfo *child = NULL;
};

struct fileInfo {
    char type;
    char file_path[PATH_MAX];
    struct directoryInfo * parentDirectory;
    struct fileInfo *prev = NULL;
    struct fileInfo *next = NULL;
};

struct directoryInfo * populateDirectory(struct directoryInfo *directoryHead){

  int count,i;
  struct direct **files;
  struct stat statbuf;
  char datestring[256];
  struct passwd pwent;
  struct passwd *pwentp;
  struct group grp;
  struct group *grpt;
  struct tm time;
  char buf[1024];

  struct directoryInfo *dirptr,*dirReturn=directoryHead;
  struct fileInfo *fileptr,*fileParent;

  count = scandir(directoryHead->dir_path, &files, one, alphasort);

  directoryHead->count = count;

  if(count > 0)
  {
    printf("\ntotal %d\n",count);

    for (i=2; i<count; i++)
    {
      if (stat(files[i]->d_name, &statbuf) == 0)
      {
        /* Print out type, permissions, and number of links. */
        string status = get_perms(statbuf.st_mode);
        printf("%10.10s",get_perms(statbuf.st_mode));

        if(status[0]=='d'){
            
            dirptr = (directoryInfo*)malloc(sizeof(directoryInfo));
            dirptr->parent = directoryHead;
            directoryHead->child = dirptr;
            dirptr->child = NULL;
            
            char *path = files[i]->d_name;
            int j=0;
            char temp[PATH_MAX];
            temp[0] = '/';
            while(path[j]!=NULL){
              temp[j+1] = path[j];
              j++;
            }
            temp[j+1] = '\0';
            printf("%s %s", directoryHead->dir_path,temp);
            snprintf(dirptr->dir_path, PATH_MAX , "%s%s", directoryHead->dir_path,temp);
                 
            directoryHead = dirptr;

        }else if(status[0]=='-'){
            
            fileptr = (fileInfo*)malloc(sizeof(fileInfo));

            char *path = files[i]->d_name;
            int j=0;
            char temp[PATH_MAX];
            temp[0] = '/';
            while(path[j]!=NULL){
              temp[j+1] = path[j];
              j++;
            }
            temp[j+1] = '\0';
            printf("%s %s",dirReturn->dir_path,temp);
            snprintf(fileptr->file_path, PATH_MAX , "%s%s", dirReturn->dir_path,temp);
            

            if(directoryHead->fileHead==NULL){

                  fileParent = fileptr;
                  directoryHead->fileHead = fileptr;
                  fileptr->parentDirectory = directoryHead;
                  fileptr->prev = NULL;
                  fileptr->next = NULL;

            }else{

                  fileParent->next = fileptr;
                  fileptr->prev = fileParent;
                  fileParent = fileptr;
                  fileptr->next = NULL;
            }
        }
         
        printf(" %d", statbuf.st_nlink);

        if (!getpwuid_r(statbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp)){
          printf(" %s", pwent.pw_name);
        }
        else
          printf(" %d", statbuf.st_uid);

        if (!getgrgid_r (statbuf.st_gid, &grp, buf, sizeof(buf), &grpt)){
          printf(" %s", grp.gr_name);
        }
        else
          printf(" %d", statbuf.st_gid);

        /* Print size of file. */
        printf(" %5d", (int)statbuf.st_size);

        localtime_r(&statbuf.st_mtime, &time);
        /* Get localized date string. */
        strftime(datestring, sizeof(datestring), "%F %T", &time);

        printf(" %s %s\n", datestring, files[i]->d_name);
      }

      free (files[i]);
    }

    free(files);
  }
  cout<<"\n"<<"return value: "<<dirReturn->dir_path<<"\n";
  return dirReturn->child;
}

int main()
{

  struct directoryInfo *directoryHead, *dirptr;
  struct fileInfo *fileptr;


  directoryHead = (directoryInfo *)malloc(sizeof(directoryInfo));  

  if(!getcwd(directoryHead->dir_path, sizeof(pathname)))
    die("Error getting pathname");
 
  directoryHead->type = 'd';
  directoryHead->parent = NULL;
  dirptr = directoryHead;

  do{
      dirptr = populateDirectory(dirptr);

  }while(dirptr!=NULL);

  dirptr = directoryHead;

  while(dirptr!=NULL){
    fileptr = dirptr->fileHead;
    printf("%s\n", dirptr->dir_path);
   // cout<<"its a directory\n";
    while(fileptr!=NULL){
      cout<<"its a file\n";
       printf("%s\n",fileptr->file_path);
       fileptr = fileptr->next; 
    }
    dirptr = dirptr->child;
  }

  return 0;
}