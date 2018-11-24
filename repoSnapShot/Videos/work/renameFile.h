#ifndef RENAME_H
#define RENAME_H

using namespace std;

void renameFile(char *oldname,char *newname){
  
  rename(oldname, newname);
  
}
#endif