#include "headerFiles.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

//#define TIOCGWINSZ      _IOR(t, 104, struct winsize)    /* get window size */
//#define TIOCSWINSZ      _IOW(t, 103, struct winsize)    /* set window size */

using namespace std;

int INDEX = 0;


string homeDirectory;
string workingDirectory;
string parentDirectory;
string searchFileName;
//static const int STDIN = 0;
//struct winsize ws,size;
//std::ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);

//struct winsize ws;
//ioctl(0,TIOCGWINSZ,&ws);


vector<pair<string,string>> presentDirectoryInfo;
vector<pair<string,int>> presentDirectory;
stack<string> traverseLeft;
stack<string> traverseRight;
vector<string> searchResult;
vector<pair<string,int>> searchDirectory;


void incrementIndex();
void decrementIndex();
void reset();
void displayStatusBar(int);
int isUpAllowed();
int isDownAllowed();
void setCursorTopRow();
void setCursorBottomRow();
void listDirectories(string,string);
void printDirectories(string);
int setCommandMode();
void displayStatusBar(int);
int setNormalMode();
void initializeVariables();
void gotoDirectory(string);

string& ltrim(string&);
string& rtrim(string&);
string& trim(string&);

//struct winsize ws;
//ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);

//printf ("lines %d\n", ws.ws_row);
//printf ("columns %d\n", ws.ws_col);


//////////////////////////////////////////////////////////// READ AND STORE DIRECTORY INFO //////////////////////////////////////////////////////////////////

static char perms_buff[30];


const char *get_perms(mode_t mode)
{
  char ftype = '?';

  if (S_ISREG(mode)) ftype = '-';
  if (S_ISLNK(mode)) ftype = 'l';
  if (S_ISDIR(mode)) ftype = 'd';
  if (S_ISBLK(mode)) ftype = 'b';
  if (S_ISCHR(mode)) ftype = 'c';
  if (S_ISFIFO(mode)) ftype = '|';

  sprintf(perms_buff, "%c%c%c%c%c%c%c%c%c%c %c%c%c", ftype,
  mode & S_IRUSR ? 'r' : '-',
  mode & S_IWUSR ? 'w' : '-',
  mode & S_IXUSR ? 'x' : '-',
  mode & S_IRGRP ? 'r' : '-',
  mode & S_IWGRP ? 'w' : '-',
  mode & S_IXGRP ? 'x' : '-',
  mode & S_IROTH ? 'r' : '-',
  mode & S_IWOTH ? 'w' : '-',
  mode & S_IXOTH ? 'x' : '-',
  mode & S_ISUID ? 'U' : '-',
  mode & S_ISGID ? 'G' : '-',
  mode & S_ISVTX ? 'S' : '-');

  return (const char *)perms_buff;
}

//char path_name[MAXPATHLEN];

void die(char *msg)
{
  perror(msg);
  exit(0);
}

static int
one (const struct dirent *unused)
{
  return 1;
}



void listDirectories(string currentDirectory,string parentDirectory){

  int count,i;
  struct dirent **files;
  struct stat statbuf;
  char datestring[256];
  struct passwd pwent;
  struct passwd *pwentp;
  struct group grp;
  struct group *grpt;
  struct tm time;
  char buf[1024];
  string temp;
  char path_name[currentDirectory.length()+1];

  strcpy(path_name,currentDirectory.c_str());


  count = scandir(path_name, &files, one, alphasort);

  if(count > 0)
  {
    //printf("total %d\n",count);

    for (i=0; i<count; ++i)
    {
      	stat(files[i]->d_name, &statbuf);
      
        /* Print out type, permissions, and number of links. */
      	string status = get_perms(statbuf.st_mode);
      	if(status[0]=='d'){
      		//sprintf(temp,"%s/%s",currentDirectory,files[i]->d_name);
      		temp = string(files[i]->d_name);
      		if(temp.length()==1&&temp[0]=='.'){
      			if(currentDirectory==homeDirectory){
      				presentDirectoryInfo.push_back(pair<string, string> (homeDirectory,homeDirectory));
      				presentDirectory.push_back(pair<string, int> (homeDirectory,1));		
      			}else{
      				presentDirectoryInfo.push_back(pair<string, string> (currentDirectory,currentDirectory));
      				presentDirectory.push_back(pair<string, int> (currentDirectory,1));
      			}
      		}
      		else if(temp.length()==2&&temp[0]=='.'&&temp[1]=='.'){
      			if(currentDirectory==homeDirectory){
      				presentDirectoryInfo.push_back(pair<string, string> (homeDirectory,homeDirectory));
      				presentDirectory.push_back(pair<string, int> (homeDirectory,1));
      			}else{
      				presentDirectoryInfo.push_back(pair<string, string> (parentDirectory,homeDirectory));
      				presentDirectory.push_back(pair<string, int> (parentDirectory,1));
      			}
      		}else{
      			temp = currentDirectory+"/"+string(files[i]->d_name);
	      		temp = trim(temp);
	      		presentDirectoryInfo.push_back(pair<string, string> (temp,currentDirectory));
				presentDirectory.push_back(pair<string, int> (temp,1));
      		}
      	}else if(status[0]!='d'){

      		//sprintf(temp,"%s/%s",currentDirectory,files[i]->d_name);
      		temp = currentDirectory+"/"+string(files[i]->d_name);
      		temp = trim(temp);
      		presentDirectoryInfo.push_back(pair<string, string> (temp,currentDirectory));
			presentDirectory.push_back(pair<string, int> (temp,0));

      	}
      	

      free (files[i]);
    }

    free(files);
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// PRINT DIRECTORY ///////////////////////////////////////////////////////////////////////////////

void printDirectories(string currentDirectory){

  int count,i;
  struct dirent **files;
  struct stat statbuf;
  char datestring[256];
  struct passwd pwent;
  struct passwd *pwentp;
  struct group grp;
  struct group *grpt;
  struct tm time;
  char buf[1024];
  string temp;
  char path_name[currentDirectory.length()+1];

  strcpy(path_name,currentDirectory.c_str());


  count = scandir(path_name, &files, one, alphasort);

  if(count > 0)
  {
    //printf("total %d\n",count);

    for (i=0; i<count; ++i)
    {
      	stat(files[i]->d_name, &statbuf);
      
        /* Print out type, permissions, and number of links. */
      	string status = get_perms(statbuf.st_mode);

        printf(" %10.10s", get_perms(statbuf.st_mode));
        printf("\t%d", statbuf.st_nlink);

        if (!getpwuid_r(statbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))
          printf("\t%s", pwent.pw_name);
        else
          printf("\t%d", statbuf.st_uid);

        if (!getgrgid_r (statbuf.st_gid, &grp, buf, sizeof(buf), &grpt))
          printf("\t%s", grp.gr_name);
        else
          printf("\t%d", statbuf.st_gid);

        /* Print size of file. */
        printf("\t%5dKB\t", (int)statbuf.st_size/1024);

        localtime_r(&statbuf.st_mtime, &time);
        /* Get localized date string. */
        strftime(datestring, sizeof(datestring), "%F %T", &time);

        printf("\t%s\t%s\n", datestring, files[i]->d_name);
      

      free (files[i]);
    }

    free(files);
  }

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////// PRINT DATASTRUCTURE /////////////////////////////////////////////////////////////////////////////

void printpresentDirectory(){
	cout<<"Present Directory\n";
	for(int i=0;i<presentDirectory.size();i++){
		cout<<presentDirectory[i].first<<"\t"<<presentDirectory[i].second<<"\n";
	}
}

void printpresentDirectoryInfo(){
	cout<<"Present Directory\n";
	for(int i=0;i<presentDirectoryInfo.size();i++){
		cout<<presentDirectoryInfo[i].first<<"\t"<<presentDirectoryInfo[i].second<<"\n";
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// SET NORMAL MODE /////////////////////////////////////////////////////////////////////////////////

int setNormalMode(){

  struct winsize ws;
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);

  int c;
  system("clear");

  if (terminal_init()) {
    if (errno == ENOTTY)
        fprintf(stderr, "This program requires a terminal.\n");
    else
        fprintf(stderr, "Cannot initialize terminal: %s.\n", strerror(errno));
  return -1;
  }
  else{
  	reset();
    listDirectories(workingDirectory,parentDirectory);
    printDirectories(workingDirectory);
    displayStatusBar(NORMALMODE);
    SetCursor(0,0);
    
  }
  //printpresentDirectory();
  //printpresentDirectoryInfo();
  //  printf("\033[1;1H");
  
  while ((c = getchar()) != EOF) {
    
    if (c == ':'){
      terminal_done();
      setCommandMode(); 
      return 0;
    }
    else if(c == BACKSPACE){
    	if(workingDirectory==homeDirectory)
    		continue;
    	size_t found = workingDirectory.find_last_of("/\\");
  		//str.substr(0,found)
    	string uplevelPath = workingDirectory.substr(0,found);
    	parentDirectory = uplevelPath.substr(0,found);
    	if(parentDirectory.size()==0)
    		parentDirectory = homeDirectory;

    	presentDirectoryInfo.clear();
		presentDirectory.clear();
		traverseLeft.push(uplevelPath);
		reset();
		listDirectories(trim(uplevelPath),parentDirectory);
		
		printDirectories(trim(uplevelPath));
		displayStatusBar(NORMALMODE);
    }
    else if ( c == HOME){
    	traverseLeft.push(homeDirectory);
    	parentDirectory = workingDirectory;
    	workingDirectory = homeDirectory;
    	presentDirectoryInfo.clear();
		presentDirectory.clear();
		reset();
		listDirectories(trim(homeDirectory),parentDirectory);
		printDirectories(trim(homeDirectory));
		displayStatusBar(NORMALMODE);
    }
    else if ( c == ARROWRIGHT) {

		if(!traverseRight.empty()){
			parentDirectory = workingDirectory;
			workingDirectory = traverseRight.top();
			traverseLeft.push(traverseRight.top());
			string targetPath = traverseRight.top();
			presentDirectoryInfo.clear();
			presentDirectory.clear();
			reset();	
			listDirectories(trim(targetPath),parentDirectory);
			printDirectories(trim(targetPath));
			traverseRight.pop();
			SetCursor(0,0);
		}

    }
    else if ( c == ARROWLEFT) {
        //printf ( "\033[D");//cursor left
		if(!traverseLeft.empty()){
			traverseRight.push(traverseLeft.top());
			traverseLeft.pop();
			parentDirectory = workingDirectory;
			workingDirectory = traverseLeft.top();
			string targetPath = traverseLeft.top();
			presentDirectoryInfo.clear();
			presentDirectory.clear();
			
			reset();	
			listDirectories(trim(targetPath),parentDirectory);	
			printDirectories(trim(targetPath));
			SetCursor(0,0);
		}if(traverseLeft.empty()){
			traverseLeft.push(homeDirectory);
		}        
    }
    else if ( c == ARROWUP) {
		if(isUpAllowed()){
			
			if(INDEX>0){
				moveCursorUp();
				INDEX--;
			}
		}
    }
    else if ( c == ARROWDOWN) {
		if(isDownAllowed()){
			if(INDEX<MAXROWINDEX){
				moveCursorDown();
				INDEX++;
			}
		}
    }
    else if (c == ENTER){
    	if(workingDirectory==homeDirectory&&INDEX==1||INDEX==0){
    		continue;
    	}
		if(presentDirectory[INDEX].second==1){
			//cout<<"sdfsd\n";
			if(workingDirectory==homeDirectory)
				parentDirectory = homeDirectory;
			else
				parentDirectory = workingDirectory;
			
			workingDirectory = presentDirectoryInfo[INDEX].first;
			traverseLeft.push(presentDirectoryInfo[INDEX].first);
			string targetPath = presentDirectoryInfo[INDEX].first;
			presentDirectoryInfo.clear();
			presentDirectory.clear();
			reset();
			listDirectories(trim(targetPath),parentDirectory);
			printDirectories(trim(targetPath));
			SetCursor(0,0);
		}
		else if(presentDirectory[INDEX].second==0){
			openFile(trim(presentDirectory[INDEX].first));
			
		}
	}
  }
  return 0;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////// SEARCH FILE ////////////////////////////////////////////////////////////////////////////////////



int searchPrint(){
    //cout<<"asfsd\n"<<file<<"\n";
    int flag = 0;
	//cout<<"sprint count: "<<searchDirectory.size()<<"\n";
	for(int i=0;i<searchDirectory.size();i++){
		//cout<<file<<" "<<directory[i].first<<"\n";
		 string temp2 = searchDirectory[i].first;
		 size_t found = temp2.find_last_of("/\\");
		 
		 string temp = temp2.substr(found+1);

		 //cout<<temp<<" "<<searchFileName<<"\n";
		if(trim(temp) == trim(searchFileName)){
			cout<<"\n"<<temp2<<"\n";
			searchResult.push_back(temp2);
			searchDirectory[i].first = "-1";
			if(flag==0)
			flag = 1;
		}
	}
	return flag;
}


int sPrint(){
	cout<<"sprint count: "<<searchDirectory.size()<<"\n";
	for(int i=0;i<searchDirectory.size();i++)
			cout<<"\n"<<searchDirectory[i].first<<searchDirectory[i].second;
			
}




void searchFiles(char *basePath)
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


void search(){
 //   cout<<"\n"<<fileName<<"\n";
	searchDirectory.clear();
    char currentPath[workingDirectory.length()+1];
            //cout<<"search: "<<searchFileName<<"\n";

    strcpy(currentPath,(char*)workingDirectory.c_str());
    printf("%s\n", currentPath);
    
    //vector<pair<string,int>> searchDirectory;

    searchFiles(currentPath);

    cout<<"count: "<<searchDirectory.size()<<"\n";

    int i=1;
    while(i<searchDirectory.size()){

    	strcpy(currentPath, (char*)(searchDirectory[i].first).c_str());
    	//cout<<string(path)<<"\n";
    	if(searchDirectory[i].second==0){
    		searchFiles(currentPath);
    		i++;	
    	}
    	
    	i++;           // cout<<"search: "<<searchFileName<<"\n";
    	cout<<i<<"\n";
    }
    //cout<<"count: "<<searchDirectory.size()<<"\n";
                //cout<<"search: "<<searchFileName<<"\n";

//    cout<<"sdfsd\n"<<fileName<<"\n";
      sPrint(); 
            //cout<<"search: "<<searchFileName<<"\n";

    if(searchPrint()){
    	traverseLeft.push(workingDirectory);
    }
   
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////// COMMAND MODE ///////////////////////////////////////////////////////////////////////////////////


int setCommandMode(){

	setCursorBottomRow();
    //displayStatusBar(COMMANDMODE);
    

    cout<<"\n\n";

    int i=0,count=0;
	string str,temp;
    
    while(getline(cin,str)){
        
        if(str[0]==ESC){
            setNormalMode();
            return 0;
        }
        else if(str.find("copy_file")==0){
            vector<string> tokens;
            char s = ' ';
            int l=0,len=0,size = str.size();
            for(int i=0;i<size;i++){
               if(str[i]==s){
                  tokens.push_back(str.substr(l,len));
                  len=0;
                  l= i+1;  
               }else{
                len++;
               } 
            }
            tokens.push_back(str.substr(l,str.size()));
                  
            //for(int i=0;i<tokens.size();i++)
             // cout<<tokens[i]<<"\n";  
            for(int i=1;i<tokens.size()-1;i++){
              copyFile(tokens[i],tokens[tokens.size()-1]);
            }

        }
        else if(str.find("copy_dir")==0){
            
        }
        else if(str.find("move_file")==0){
            
        }
        else if(str.find("move_dir")==0){
            
        }
        else if(str.find("rename")==0){
            i=0;
            while(str[i]!=' '){
                i++;
            }
            i++;
            temp = str.substr(i);
            i=0;
            count = 0;
            while(temp[i]!=' '){
                i++;
                count++;
            }
            char oldname[count];
            i=0;
            while(temp[i]!=' '){
                oldname[i] = temp[i];
                i++;
            }
            oldname[i] = '\0';
            i++;
            temp = temp.substr(i);
            char newname[temp.length()];
            i=0;
            while(i<temp.length()){
                newname[i] = temp[i];
                //cout<<c[i];
                i++;
            }
            newname[i]='\0';
            renameFile(oldname,newname);
        }
        else if(str.find("create_dir")==0){
            i=0;
            while(str[i]!=' '){
                i++;
            }
            i++;
            temp = str.substr(i);
            
            createDirectory(temp);
        }
        else if(str.find("create_file")==0){

            i=0;
            while(str[i]!=' '){
                i++;
            }
            i++;
            temp = str.substr(i);
            i=0;
            count = 0;
            while(temp[i]!=' '){
                i++;
                count++;
            }
            char file[count];
            i=0;
            while(temp[i]!=' '){
                file[i] = temp[i];
                i++;
            }
            file[i] = '\0';
            string fileName(file);
            i++;
            temp = temp.substr(i);
            char dire[temp.length()];
            i=0;
            while(i<temp.length()){
                dire[i] = temp[i];
                //cout<<c[i];
                i++;
            }
            dire[i] = '\0';
            string directory(dire);

            createFile(file,dire);
        }
        else if(str.find("delete_file")==0){
            i=0;
            while(str[i]!=' '){
                i++;
            }
            i++;   
            temp = str.substr(i);
            char c[temp.length()];
            //cout<<"temp.length()"<<temp.length()<<"\n";
            i=0;
            while(i<temp.length()){
                c[i] = temp[i];
                cout<<c[i];
                i++;
            }
            c[i]='\0';
            cout<<"\n"<<temp;
            deleteFile(c);
        }
        else if(str.find("delete_dir")==0){
            i=0;
            while(str[i]!=' '){
                i++;
            }
            i++;   
            temp = str.substr(i);
            char c[temp.length()];
            //cout<<"temp.length()"<<temp.length()<<"\n";
            i=0;
            while(i<temp.length()){
                c[i] = temp[i];
                cout<<c[i];
                i++;
            }
            c[i]='\0';
            cout<<"\n"<<temp;
            delete_folder_tree(c);
        }
        else if(str.find("goto")==0){
        	vector<string> tokens;
            char s = ' ';
            int l=0,len=0,size = str.size();
            for(int i=0;i<size;i++){
               if(str[i]==s){
                  tokens.push_back(str.substr(l,len));
                  len=0;
                  l= i+1;  
               }else{
                len++;
               } 
            }
            tokens.push_back(str.substr(l,str.size()));
            gotoDirectory(tokens[1]);  
        }
        else if(str.find("search")==0){
        	vector<string> tokens;
            char s = ' ';
            int l=0,len=0,size = str.size();
            for(int i=0;i<size;i++){
               if(str[i]==s){
                  tokens.push_back(str.substr(l,len));
                  len=0;
                  l= i+1;  
               }else{
                len++;
               } 
            }
            tokens.push_back(str.substr(l,str.size()));
            searchFileName = tokens[1];
            cout<<"search: "<<searchFileName<<"\n";
            search();
        }
        else if(str.find("snapshot")==0){

        	vector<string> tokens;
            char s = ' ';
            int l=0,len=0,size = str.size();
            for(int i=0;i<size;i++){
               if(str[i]==s){
                  tokens.push_back(str.substr(l,len));
                  len=0;
                  l= i+1;  
               }else{
                len++;
               } 
            }
            tokens.push_back(str.substr(l,str.size()));
            //cout<<tokens[0]<<" "<<tokens[1]<<" "<<tokens[2]<<"\n";
        	int pid = fork();

		    if (pid == 0) {
		    	try{
		    		execl("./snapshot",tokens[0].c_str(),tokens[1].c_str(),tokens[2].c_str());		
		    	}catch(exception e){
		    		
		    	}
		      
		      exit(1);
		    }

        }
            
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////// MAIN FUNCTION //////////////////////////////////////////////////////////////////////////////////

int main(){

	// printf ("lines %d\n", ws.ws_row);
	 //printf ("columns %d\n", ws.ws_col);

	initializeVariables();
	cout<<homeDirectory<<"\n";
	if(setNormalMode()==-1)
		printf("Some error happened\n");
	return 0;	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// GOTO DIRECTORY /////////////////////////////////////////////////////////////////////////////

void gotoDirectory(string dir){
	reset();
	//cout<<dir<<"\n";
	dir = homeDirectory+dir;
	workingDirectory = dir;
	traverseLeft.push(dir);
	setNormalMode();
	//cout<<dir<<"\n";
	traverseLeft.push(dir);
	presentDirectoryInfo.clear();
	presentDirectory.clear();
	//listDirectories(dir);
	SetCursor(1,0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// SET CURSOR BOTTOM ROW //////////////////////////////////////////////////////////////////////

void setCursorBottomRow(){
	SetCursor(BOTTOMROW,0);
	//CURR_ROW.ws_row=BOTTOMROW;
	//CURR_ROW.ws_col=0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////// SET CURSOR TOP ROW //////////////////////////////////////////////////////////////////////

void setCursorTopRow(){
	SetCursor(FIRSTROW,0);
	//CURR_ROW.ws_row=FIRSTROW;
	//CURR_ROW.ws_col=0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// RESET CURSOR //////////////////////////////////////////////////////////////////////////////


void reset()
{
	ClearScreen();
	INDEX = 0;
	SetCursor(0,0);
	//CURR_ROW.ws_row=FIRSTROW;
	//CURR_ROW.ws_col=0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// INITIALIZE VARIABLE ///////////////////////////////////////////////////////////////////////

void initializeVariables(){

   char cwd[PATH_MAX];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
      // printf("Current working dir: %s\n", cwd);
   } else {
       perror("getcwd() error");
       
   }
   homeDirectory = string(cwd);
   workingDirectory = string(cwd);
   parentDirectory = string(cwd);
   traverseLeft.push(cwd);
   //cout<<homeDirectory<<"\n";
   INDEX = 0;
   //reset();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// IS UP MOVEMENT ALLOWED INDEX ////////////////////////////////////////////////////////////////////

int isUpAllowed(){
	if(INDEX>MINROWINDEX)
		return TRUE;
	else
		return FALSE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// IS DOWN MOVEMENT ALLOWED INDEX //////////////////////////////////////////////////////////////////

int isDownAllowed(){
	if(INDEX<MAXROWINDEX)
		return TRUE;
	else
		return FALSE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////// INCREMENT INDEX ////////////////////////////////////////////////////////////////////////////////

void incrementIndex(){
	INDEX++;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// DECREMENT INDEX ////////////////////////////////////////////////////////////////////////////////

void decrementIndex(){
	INDEX--;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// DISPLAY STATUS BAR //////////////////////////////////////////////////////////////////////////////

void displayStatusBar(int MODE){
	if(MODE==NORMALMODE){
		SetCursor(STATUSROW,0);
		//CURR_ROW.ws_row=STATUSROW;
		//CURR_ROW.ws_col=0;
		//cout<<"**************************************************************************************************************************\n";
		//cout<<"NORMAL MODE\n";
		//cout<<"**************************************************************************************************************************\n";
		SetCursor(1,0);
		INDEX = 0;
	}else if(MODE == COMMANDMODE){
		//reset();
		listDirectories(workingDirectory,parentDirectory);
		SetCursor(STATUSROW,0);
		//CURR_ROW.ws_row=STATUSROW;
		//CURR_ROW.ws_col=0;
		//cout<<"**************************************************************************************************************************\n";
		//cout<<"COMMAND MODE\n";
		//cout<<"**************************************************************************************************************************\n";
		SetCursor(STATUSROW+1,0);
		
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////// TRIM STRINGS ////////////////////////////////////////////////////////////////////////////////////////

string& ltrim(std::string& s)
{
    auto it = std::find_if(s.begin(), s.end(),
                            [](char c) {
                                return !std::isspace<char>(c, std::locale::classic());
                            });
    s.erase(s.begin(), it);
    return s;
}

string& rtrim(std::string& s)
{
    auto it = std::find_if(s.rbegin(), s.rend(),
                        [](char c) {
                            return !std::isspace<char>(c, std::locale::classic());
                        });
    s.erase(it.base(), s.end());
    return s;
}

string& trim(std::string& s)
{
    return ltrim(rtrim(s));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////