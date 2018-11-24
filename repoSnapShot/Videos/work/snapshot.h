
#ifndef SNAPSHOT_H
#define SNAPSHOT_H

using namespace std;


void listFilesRecursively(char *basePath,vector<pair<string, int>> &directory)
{
    char path[PATH_MAX];
    string temp;
    int flag=1;
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    if(dir){
    	temp = string(basePath);
    	directory.push_back(pair<string, int> (temp,1));

    while ((dp = readdir(dir)) != NULL)
    {
    	string str;

    	if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            strcpy(path, dp->d_name);
            str = string(path);
    		directory.push_back(pair<string, int> (str,0));
    	}
    }
    	closedir(dir);
	}
}

void createDump(vector<pair<string, int>> directory){


	ofstream dumpfile;
	dumpfile.open ("snapshot_dump", std::ofstream::out);
	for(int i=0;i<directory.size();i++){
 		if(directory[i].second==1)
			dumpfile<<"\n\nDirectory: "<<directory[i].first<<"\n\n";
		else
			dumpfile<<directory[i].first<<"\n";
	}	
	dumpfile.close();

}

void snapshot(string snapDirectory)
{
    char path[snapDirectory.length()+1];

    strcpy(path,snapDirectory.c_str());

    vector<pair<string,int>> directory;


    listFilesRecursively(path,directory);
    int i=1;
    while(i<directory.size()){

    	strcpy(path, (directory[i].first).c_str());
    	if(directory[i].second==0)
    	listFilesRecursively(path,directory);
    	i++;
    }
    
    createDump(directory);
}




#endif