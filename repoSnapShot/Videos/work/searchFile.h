#ifndef SEARCHFILE_H
#define SEARCHFILE_H

using namespace std;



void searchPrint(vector<pair<string, int>> searchDirectory,string file){
    cout<<"asfsd\n"<<file<<"\n";
	for(int i=0;i<searchDirectory.size();i++){
		//cout<<file<<" "<<directory[i].first<<"\n";	
		size_t found = searchDirectory[i].first.find(file);
		if(found!=std::string::npos){
			cout<<"\n"<<searchDirectory[i].first<<"\n";
		}
	}
}



void listFiles(char *basePath,vector<pair<string, int>> &searchDirectory)
{
    char path[PATH_MAX];
    string temp;
    int flag=1;
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    if(dir){
    	temp = string(basePath);
    	//directory.push_back(temp);
    	searchDirectory.push_back(pair<string, int> (temp,1));

    while ((dp = readdir(dir)) != NULL)
    {
    	string str;

    	if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            //strcpy(path, basePath);
            //strcat(path, "/");
            strcpy(path, dp->d_name);
            //printf("%s\n",path );
            str = string(basePath)+"/"+string(path);
    		searchDirectory.push_back(pair<string, int> (str,0));
    	}
    }
    	closedir(dir);
	}
}


void search(string fileName){
    cout<<"\n"<<fileName<<"\n";
    char path[fileName.length()+1];

    strcpy(path,fileName.c_str());
    printf("%s\n", path);
    
    vector<pair<string,int>> searchDirectory;

    listFiles(path,searchDirectory);
    int i=1;
    while(i<searchDirectory.size()){

    	strcpy(path, (searchDirectory[i].first).c_str());
    	//cout<<string(path)<<"\n";
    	if(searchDirectory[i].second==0)
    	listFiles(path,searchDirectory);
    	i++;
    }
    cout<<"sdfsd\n"<<fileName<<"\n";
    searchPrint(searchDirectory,fileName);
   
}


#endif