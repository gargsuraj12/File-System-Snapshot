#ifndef CREATEFILE_H
#define CREATEFILE_H

using namespace std;

void createFile(char* fileName,char* directory){
  char* pathFile;
  sprintf(pathFile, "%s/%s", directory,fileName );
  int filedescriptor = open(pathFile, O_WRONLY | O_APPEND | O_CREAT);   
  chmod(pathFile, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);
}

#endif