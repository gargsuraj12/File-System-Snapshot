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

int main()

{
    string filename;

    printf("Enter the filename to be opened \n");

    getline(cin,filename);

    char file[filename.length()+1];

    for(int i=0;i<filename.length();i++)
        file[i] = filename[i];

    file[filename.length()+1] = NULL;

    int pid = fork();

    if (pid == 0) {
      execl("/usr/bin/xdg-open", "xdg-open", file, (char *)0);
      exit(1);
    }

    return 0;

}