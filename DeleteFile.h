
// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <dirent.h>
// #include <sys/stat.h>
// #include <unistd.h>
// #include <iostream>
#define ERRORDIRNOTEXIST -110
#define ERRORWHILEDELETING -111
#define ERRORPERMISSIONWHILEDELETING -112
#define ERRORPATHNOTEXIST -113
#define SUCCESS 100

// #include<string>
// #include<bits/stdc++.h>
// using namespace std;

class DeleteFunctionality{
    public:
        int removesingleFile(const char path[] ){
            //char *full_path; 
            if (unlink(path) == 0)
            {
                    //printf("Removed a file: %s\n", path);
                return SUCCESS;
            }
            else
            {
                // printf("Can`t remove a file: %s\n", path);
                return ERRORWHILEDELETING;
            }
        }

        int permitToDelete  = 1;

        int removedirectory(char *path)
        {

            //if(permitToDelete==0) return ERRORPERMISSIONWHILEDELETING; 
            printf(" remove a directory: %s\n", path);
            size_t pathlength;
            
            DIR *directory;
            struct stat stat_path, stat_entry;
            struct dirent *entry;

            stat(path, &stat_path);

            // if path does not exists or is not dir - //exit with status -1
            if (S_ISDIR(stat_path.st_mode) == 0) {
                printf(" not a directory: %s\n", path);
                //fprintf(stderr, "%s: %s\n", "Is not directory", path);
                if(S_ISREG(stat_path.st_mode))
                    removesingleFile(path);
                ////exit(-1);
                return SUCCESS;    
            }

            // if not possible to read the directory for this user
            if ((directory = opendir(path)) == NULL) {
                //printf(" opendir directory: %s\n", path);
                //fprintf(stderr, "%s: %s\n", "Can`t open directory", path);
                ////exit(-1);
                return ERRORPERMISSIONWHILEDELETING;
            }
            char *actual_path;
            pathlength = strlen(path);
            
            while ((entry = readdir(directory)) != NULL) {
                printf("inside while: %s\n", entry->d_name);
                if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                    continue;

                actual_path = (char *)calloc(pathlength + strlen(entry->d_name) + 1, sizeof(char));
                strcpy(actual_path, path);
                strcat(actual_path, "/");
                strcat(actual_path, entry->d_name);

                stat(actual_path, &stat_entry);
                printf("inside while actual pathh : %s\n", actual_path);

                // recursively remove a nested directory
                if (S_ISDIR(stat_entry.st_mode) != 0) {
                    removedirectory(actual_path);
                    continue;
                }

                // remove a file object
                if (unlink(actual_path) == 0)
                {
                    printf("Removed a file: %s\n", actual_path);
                    //return SUCCESS;
                }
                else
                {
                    printf("Can`t remove a file: %s\n", actual_path);
                    //return ERRORWHILEDELETING;
                }
            }

            if (rmdir(path) == 0)
            {
                printf("Removed a directory: %s\n", path);
                return SUCCESS;
            }
            else
            {
                printf("Can`t remove a directory: %s\n", path);
                return ERRORWHILEDELETING;
            }
            permitToDelete = 1;
            closedir(directory);
            return SUCCESS;
        }

};
// int main(const int argc, char const *argv[])
// {
//     char p[1024];
//     scanf("%s",p);
//     DeleteFunctionality obj;
//     obj.removedirectory(p);

//     return 0;
// }

