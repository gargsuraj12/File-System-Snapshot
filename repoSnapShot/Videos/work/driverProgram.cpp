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


#define ESC          27
#define INSERT       50
#define DELETE       51
#define PGUP         53
#define PGDN         54
#define ARROWRIGHT   67
#define ARROWLEFT    68
#define ARROWUP      65
#define ARROWDOWN    66
#define END          70
#define HOME         72
#define OTHER        79
#define BRACKETLEFT  91
#define TILDE       126
#define BACKSPACE   127

int rmdir(const char *pathname);
static int    terminal_descriptor = -1;
static struct termios terminal_original;
static struct termios terminal_settings;

void setCommandMode();

/* Restore terminal to original settings
*/
static void terminal_done(void)
{
    if (terminal_descriptor != -1)
        tcsetattr(terminal_descriptor, TCSANOW, &terminal_original);
    terminal_descriptor  = -1;
}
 

static int terminal_init(void)
{
    struct sigaction act;
 
    /* Already initialized? */
    if (terminal_descriptor != -1)
        return errno = 0;
 
    /* Which standard stream is connected to our TTY? */
    if (isatty(STDERR_FILENO))
        terminal_descriptor = STDERR_FILENO;
    else
    if (isatty(STDIN_FILENO))
        terminal_descriptor = STDIN_FILENO;
    else
    if (isatty(STDOUT_FILENO))
        terminal_descriptor = STDOUT_FILENO;
    else
        return errno = ENOTTY;
 
    /* Obtain terminal settings. */
    if (tcgetattr(terminal_descriptor, &terminal_original) ||
        tcgetattr(terminal_descriptor, &terminal_settings))
        return errno = ENOTSUP;
 
    /* Disable buffering for terminal streams. */
    if (isatty(STDIN_FILENO))
        setvbuf(stdin, NULL, _IONBF, 0);
    if (isatty(STDOUT_FILENO))
        setvbuf(stdout, NULL, _IONBF, 0);
    if (isatty(STDERR_FILENO))
        setvbuf(stderr, NULL, _IONBF, 0);
 
    
    /* Let BREAK cause a SIGINT in input. */
    terminal_settings.c_iflag &= ~IGNBRK;
    terminal_settings.c_iflag |=  BRKINT;
 
    /* Ignore framing and parity errors in input. */
    terminal_settings.c_iflag |=  IGNPAR;
    terminal_settings.c_iflag &= ~PARMRK;
 
    /* Do not strip eighth bit on input. */
    terminal_settings.c_iflag &= ~ISTRIP;
 
    /* Do not do newline translation on input. */
    terminal_settings.c_iflag &= ~(INLCR | IGNCR | ICRNL);

 
    /* Use 8-bit characters. This too may affect standard streams,
     * but any sane C library can deal with 8-bit characters. */
    terminal_settings.c_cflag &= ~CSIZE;
    terminal_settings.c_cflag |=  CS8;
 
    /* Enable receiver. */
    terminal_settings.c_cflag |=  CREAD;
 
    /* Let INTR/QUIT/SUSP/DSUSP generate the corresponding signals. */
    terminal_settings.c_lflag |=  ISIG;
 
    /* Enable noncanonical mode.
     * This is the most important bit, as it disables line buffering etc. */
    terminal_settings.c_lflag &= ~ICANON;
 
    /* Disable echoing input characters. */
    terminal_settings.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
 
    /* Disable implementation-defined input processing. */
    terminal_settings.c_lflag &= ~IEXTEN;
 
    /* To maintain best compatibility with normal behaviour of terminals,
     * we set TIME=0 and MAX=1 in noncanonical mode. This means that
     * read() will block until at least one byte is available. */
    terminal_settings.c_cc[VTIME] = 0;
    terminal_settings.c_cc[VMIN] = 1;
 
    /* Set the new terminal settings.
     * Note that we don't actually check which ones were successfully
     * set and which not, because there isn't much we can do about it. */
    tcsetattr(terminal_descriptor, TCSANOW, &terminal_settings);
 
    /* Done. */
    return errno = 0;
}

//############################################################### LS FUNCTION ################################################################################
 
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

char pathname[MAXPATHLEN];

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


int listDirectories(){

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

  if(!getcwd(pathname, sizeof(pathname)))
    die("Error getting pathnamen");

  count = scandir(pathname, &files, one, alphasort);

  if(count > 0)
  {
    printf("total %d\n",count);

    for (i=0; i<count; ++i)
    {
      if (stat(files[i]->d_name, &statbuf) == 0)
      {
        /* Print out type, permissions, and number of links. */
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
      }

      free (files[i]);
    }

    free(files);
  }

   /* DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d)
    {
	while ((dir = readdir(d)) != NULL)
        {
            printf("%s  %c  %d\n", dir->d_name);
        }
        closedir(d);
    }
    return 0;*/
}


//################################################################################################################################################

//################################################################## OPEN FILE ###################################################################

void openFile(string filename){

    string str= "xdg-open ";

    str.append(filename);

    char file[str.length()];

    for(int i=0;i<str.length();i++)
        file[i] = str[i];

    system(file);

}

//################################################################################################################################################


int setNormalMode(){
	int c;
    system("clear");

	if (terminal_init()) {
		if (errno == ENOTTY)
		    fprintf(stderr, "This program requires a terminal.\n");
		else
		    fprintf(stderr, "Cannot initialize terminal: %s.\n", strerror(errno));
	return -1;
	}else{
		listDirectories();
	}

	printf("\n\nPress : to enter command mode.\n");

	while ((c = getchar()) != EOF) {
		if (c == ':'){
            terminal_done();
			setCommandMode();	
			break;
		}
        if ( c == ARROWRIGHT) {
                printf ( "\033[C");//cursor right
             
        }
        if ( c == ARROWLEFT) {
            
                printf ( "\033[D");//cursor left
            
        }
        if ( c == ARROWUP) {
            
                printf ( "\033[A");//cursor right
            
        }
        if ( c == ARROWDOWN) {
            
                printf ( "\033[B");//cursor left
            
        }
	}
 
    printf("Done.\n");
    return 0;
}

int remove_directory(const char *path)
{
             cout<<"im here\n";
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d)
   {
      struct dirent *p;

      r = 0;

      while (!r && (p=readdir(d)))
      {
          int r2 = -1;
          char *buf;
          size_t len;

          /* Skip the names "." and ".." as we don't want to recurse on them. */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
          {
             continue;
          }

          len = path_len + strlen(p->d_name) + 2; 
          buf = (char*)malloc(len);

          if (buf)
          {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);

             if (!stat(buf, &statbuf))
             {
                if (S_ISDIR(statbuf.st_mode))
                {
                   r2 = remove_directory(buf);
                }
                else
                {
                   r2 = unlink(buf);
                }
             }

             free(buf);
          }

          r = r2;
      }

      closedir(d);
   }

   if (!r)
   {
      r = rmdir(path);
   }

   return r;
}


void delete_folder_tree(const char *dirname)
{
DIR *dir;
int i=0;
cout<<"\n";
while(dirname[i]!=NULL){
cout<<dirname[i];
i++;
}
cout<<i<<"\n";
struct dirent *entry;
char path[PATH_MAX];

if (path == NULL) {
return;
}
dir = opendir(dirname);
if (dir == NULL) {
printf("Error opendir()");
return;
}

while ((entry = readdir(dir)) != NULL) {
if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
snprintf(path, (size_t) PATH_MAX, "%s/%s", dirname, entry->d_name);
if (entry->d_type == DT_DIR) {
delete_folder_tree(path);
} else {
unlink(path);
}
}

}
closedir(dir);

rmdir(dirname);

}


void renameFile(char *oldname,char *newname){
  int ret;
   //char oldname[] = "file.txt";
   //char newname[] = "newfile.txt";
  cout<<"\n";
  int i=0;
  while(oldname[i]!=NULL){
    cout<<oldname[i];
    i++;
  }
  //cout<<"\n"<<i<<"\n";
  
  i=0;
  while(newname[i]!=NULL){
    cout<<newname[i];
    i++;
  }
  //cout<<"\n"<<i<<"\n";
  

  rename(oldname, newname);
  
}

void createFile(char* fileName,char* directory){
  char* pathFile;
  sprintf(pathFile, "%s/%s", directory,fileName );
  int filedescriptor = open(pathFile, O_WRONLY | O_APPEND | O_CREAT);   
  chmod(pathFile, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);
}

void setCommandMode(){

	//terminal_done();
  int i=0,count=0;
	string str,temp;
    //printf("\e[1;1H\e[2J");
    //system("clear");
    printf("\n\nTo go back to normal mode press ESC button\n");

    while(getline(cin,str)){
        if(str[0]==ESC){
            setNormalMode();
            break;
        }else if(str[0]=='Q'||str[0]=='q'){
            break;
        }else if(str.find("copy")==0){
            //done
        }else if(str.find("move")==0){
            
        }else if(str.find("rename")==0){
            i=0;
            while(str[i]!=' '){
                i++;
            }
            i++;
            temp = str.substr(i);
            i=0;
            count = 0;
            while(temp[i]!=' '){
                i++;
                count++;
            }
            char oldname[count];
            i=0;
            while(temp[i]!=' '){
                oldname[i] = temp[i];
                i++;
            }
            oldname[i] = '\0';
            i++;
            temp = temp.substr(i);
            char newname[temp.length()];
            i=0;
            while(i<temp.length()){
                newname[i] = temp[i];
                //cout<<c[i];
                i++;
            }
            newname[i]='\0';
            renameFile(oldname,newname);
        }else if(str.find("create_dir")==0){

        }else if(str.find("create_file")==0){

            i=0;
            while(str[i]!=' '){
                i++;
            }
            i++;
            temp = str.substr(i);
            i=0;
            count = 0;
            while(temp[i]!=' '){
                i++;
                count++;
            }
            char file[count];
            i=0;
            while(temp[i]!=' '){
                file[i] = temp[i];
                i++;
            }
            file[i] = '\0';
            string fileName(file);
            i++;
            temp = temp.substr(i);
            char dire[temp.length()];
            i=0;
            while(i<temp.length()){
                dire[i] = temp[i];
                //cout<<c[i];
                i++;
            }
            dire[i] = '\0';
            string directory(dire);

            createFile(file,dire);
        }else if(str.find("delete_file")==0){

        }else if(str.find("delete_dir")==0){
            i=0;
            while(str[i]!=' '){
                i++;
            }
            i++;   
            temp = str.substr(i);
            char c[temp.length()];
            //cout<<"temp.length()"<<temp.length()<<"\n";
            i=0;
            while(i<temp.length()){
                c[i] = temp[i];
                cout<<c[i];
                i++;
            }
            c[i]='\0';
            cout<<"\n"<<temp;
            delete_folder_tree(c);
        }else if(str.find("goto")==0){

        }else if(str.find("search")==0){

        }
    }

    /*
	printf("\n\nTo go back to normal mode press n\n");
	while ((*c = getc(stdin)) != EOF) {
		if (*c == 'N' || *c == 'n'){
			setNormalMode();	
			break;
		}else if (*c == 3 || *c == 'Q' || *c == 'q'){
            break;
		}
	}*/
 }

int main(){
	
	printf("\nDrirectory Structure\n\n");
	if(setNormalMode()==-1)
		printf("Some error happened\n");
	return 0;	
}

