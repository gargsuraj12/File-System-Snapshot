#ifndef COPYFILE_H
#define COPYFILE_H

using namespace std;

void copyFileHelper(char source[], char destination[]) {

    int src_fd, dst_fd, n, err;
    unsigned char buffer[4096];
    char * src_path, *dst_path;

    src_path = source;
    dst_path = destination;


    
    src_fd = open(src_path, O_RDONLY);
    dst_fd = open(dst_path, O_CREAT | O_WRONLY);

    while (1) {
        err = read(src_fd, buffer, 4096);
        if (err == -1) {
            printf("Error reading file.\n");
            exit(1);
        }
        n = err;

        if (n == 0) break;

        err = write(dst_fd, buffer, n);
        if (err == -1) {
            printf("Error writing to file.\n");
            exit(1);
        }
    }
    chmod(dst_path, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);
    close(src_fd);
    close(dst_fd);
    
}

string SplitFilename (string& str)
{
  size_t found = str.find_last_of("/\\");
  return str.substr(found+1);
}


void copyFile(string source,string target)
{

    string temp = SplitFilename(source);
    
    
    target = target+"/"+temp;
 
    char s[source.length()+1];
    strcpy(s,source.c_str());
    
    char d[target.length()+1];
    strcpy(d,target.c_str());
 	//cout<<"source: "<<source<<"\n";
    //cout<<"target: "<<target<<"\n";
    
    copyFileHelper(s,d);
}

#endif