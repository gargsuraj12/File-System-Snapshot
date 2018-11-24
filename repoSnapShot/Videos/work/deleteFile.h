#ifndef DELETEFILE_H
#define DELETEFILE_H

using namespace std;

void deleteFile(char filename[])
{
    FILE *fp;
    int status;
    fp = fopen(filename, "r");
    
    if(fp == NULL)
    {
        printf("Error:  file not found! check the directory!!nn");
    }
    fclose(fp);
    status = remove(filename);
    
    if( status == 0 )
    printf("%s file deleted successfully.n",filename);
    else
    {
        printf("Unable to delete the filen");
        perror("Error");
    }

}

#endif