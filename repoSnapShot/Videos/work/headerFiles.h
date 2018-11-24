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
#include <fstream>
#include <bits/stdc++.h>
#include <iomanip>
#include <ctime>

#include "normalMode.h"
#include "fileOpen.h"
#include "deleteFolder.h"
#include "createFile.h"
#include "renameFile.h"
#include "deleteFile.h"
#include "copyFile.h"
#include "createDirectory.h"
#include "snapshot.h"
#include "searchFile.h"

#define ESC          27
#define INSERT       50
#define DELETE       51
#define PGUP         53
#define PGDN         54
#define ENTER        13
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

#define FIRSTROW 0
#define BOTTOMROW 40
#define FIRSTCOL 0
#define MAXROWINDEX 30
#define MINROWINDEX 0
#define STATUSROW 54
#define NORMALMODE 1
#define COMMANDMODE 0
#define TRUE 1
#define FALSE 0

#define ClearScreen() printf("\033[2J")
#define moveCursorUp() printf("\033[A")
#define SetCursor(x,y) printf("\033[%d;%dH",x,y)
#define moveCursorLeft() printf("\033[C")
#define moveCursorRight() printf("\033[D")
#define moveCursorDown() printf("\033[B")




#ifndef HEADER_H
#define HEADER_H

#endif  

