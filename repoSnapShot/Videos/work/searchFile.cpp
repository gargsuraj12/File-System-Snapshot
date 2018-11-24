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

char pathname[MAXPATHLEN];

int main(int argc, char *argv[])
{
	DIR *dp;
	struct dirent *dirp;

	if(!getcwd(pathname, sizeof(pathname)))
	    printf("Error getting pathnamen");

	if ((dp = opendir(pathname)) == NULL)
	{
		printf("can't open %s", pathname);
		exit(1);
	}
	
	while ((dirp = readdir(dp)) != NULL)
	{
		if(!strcmp(dirp->d_name,argv[1]))
		printf("%s\n", dirp->d_name);
	}
	
	closedir(dp);
	exit(0);
}

