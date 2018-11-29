#include "library.h"
#include "HelperClass.h"
#include "DeleteFile.h"
#include "initialCopy.h"
#include "createSnapshot.h"
#include "rsync.h"

#define MDLogFilePath "./LogFile/logfile.txt"

using namespace std;

class SyncData{

public:

	int toLog = 1;

	string SRCPATH = "";
	string DESTPATH = "";

	char * getCurrentTime()
	{
	    std::time_t result = std::time(nullptr);
	    return std::asctime(std::localtime(&result));
	}

	bool writeLog(string Data,int flag)
	{
		if(toLog==0)
			return false;

		bool success = false;
		if(Data!="")
		{
			std::ofstream out;
	 		out.open(MDLogFilePath, std::ios::app);
	 		string strDate(getCurrentTime());
	 		if(flag == 1)
	 		{
	 			out << "Information : " ;
	 		}
	 		if(flag == -1)
	 		{
	 			out << "Error : " ;
	 		}
	 		out <<  strDate << ":" << Data << endl;
			out.close();
		}
		return success;
	}


	int getChunkSizeOfFile(string filePath){
		struct stat infofile;
            // strcpy(temppath,source);
        int status = stat((char *)filePath.c_str(),&infofile);
        if(status != 0){
        	cout<<"Unable to get file stat for"<<filePath<<endl;
        	return -1;
        }else{
        	cout<<"file Size is: "<<infofile.st_size<<endl;
        }
        return ceil(sqrt(infofile.st_size));
	}

	int createSnapshotFunctionality(string sourcePath,string destinationPath){

		CreateSnapShotClass createSnapShotClassObj;
		createSnapShotClassObj.prepareMetadataForSnapshot(toCharArrayFromString(sourcePath),toCharArrayFromString(destinationPath),1);
		createSnapShotClassObj.CreateSnapshotFile(toCharArrayFromString(sourcePath),toCharArrayFromString(destinationPath));

		return 0;
	}

	bool static isFile(const struct compareSnapshot a,const struct compareSnapshot b){

		if(a.details.isFile==1&&b.details.isFile==0)
			return false;

		return true;
	}

	vector<struct compareSnapshot> sortList(vector<struct compareSnapshot> diffList){

		cout << "Before Sorting " << endl;

		for(int i=0;i<diffList.size();i++)
		{
			cout<< diffList[i].details.fullQualifiedPath<<" "<<diffList[i].operationType << " " << diffList[i].details.isFile <<
			diffList[i].details.ownership << "" << 
			diffList[i].details.timeStamp << "" <<
			diffList[i].details.accessRights << "" <<
			diffList[i].newTimeStamp << " " << diffList[i].oldTimeStamp << " " << "\n";
		}

		cout << "Before Sorting End " << endl << endl;

		sort(diffList.begin(),diffList.end(), isFile);
		
		cout << "After Sorting " << endl;
		for(int i=0;i<diffList.size();i++)
		{
		cout<< diffList[i].details.fullQualifiedPath<<" "<<diffList[i].operationType<<"\n";
		}
		cout << "After Sorting End" << endl << endl;
		return diffList;
	}	

	vector<struct compareSnapshot> removeRedundantEntries(vector<struct compareSnapshot> diffList){

		// cout<<"inside removeRedundantEntries\n";
		// for(int i=0;i<diffList.size();i++){
		// 	cout<< diffList[i].details.fullQualifiedPath<<" "<<diffList[i].operationType<<"\n";
		// }


		vector<int> toDelete;

		for(int i=0;i<diffList.size();i++){

			for(int j=0;j<diffList.size();j++){

				if(diffList[j].details.isFile==false){

					std::size_t found = diffList[i].details.fullQualifiedPath.find(diffList[j].details.fullQualifiedPath);
					if (found!=std::string::npos&&diffList[i].details.fullQualifiedPath.length()!=diffList[j].details.fullQualifiedPath.length()){
						// cout<<"Here\n";
						toDelete.push_back(i);
						break;
					}

				}

			}
		}

		vector<struct compareSnapshot> modifiedDiffList;

		for(int i=0;i<diffList.size();i++){

			bool flag = false;

			for(int j=0;j<toDelete.size();j++){
				if(i==toDelete[j]){
					flag = true;
					break;
				}
			}

			if(flag==false){
				modifiedDiffList.push_back(diffList[i]);
			}

		}
		
		return modifiedDiffList;
	}

	vector<struct snapshotDetails> readSnapshot(string path){

		writeLog("Entering into readSnapshot",1);
		writeLog(path,1);
		// writeLog(destinationPath,1);

		ifstream snapshotFile(path);

		vector<struct snapshotDetails> detailsList;

		struct snapshotDetails listItem;

		string line,sPath,dPath;
		
		int lineNo = 1;

		// cout<<"File path: "<<path;

		if(snapshotFile.is_open()){

			while(getline(snapshotFile,line)){
				// cout<<"Line number: "<<lineNo<<" line: "<<line<<"\n";
				if(lineNo==1){
					
					sPath = line;
					SRCPATH = sPath;
					
				}else if(lineNo == 2){

					dPath = line;
					DESTPATH = dPath;
					// cout<<"DESTPATH dfgkdfkgdlfg: "<<DESTPATH<<" liine no: "<<line<<"\n";

				}else if(lineNo >= 4){

					char *fileLine = new char [line.length()+1];
					strcpy(fileLine, line.c_str());
					
					vector<string> tokens = split(fileLine,"\t");

					if(tokens.size()>0 && tokens[0]==".snapshot")
					{
						cout<<"ooooo SNAPSHOT FILE REMOVED \n";
						continue;
					}


					if(tokens.size()!=5){

						cout<<"oooooooooooooooooooooooooooooooooooooooooooooooooo\n";
						for(int i=0;i<tokens.size();i++)
							cout<<tokens[i]<<"\n";
						cout<<"oooooooooooooooooooooooooooooooooooooooooooooooooo\n";
					}
					// listItem.fullQualifiedPath = sPath +"/"+tokens[0];
					if(tokens.size()>=5){
						//Unix V.pdf	file	prakashjha	rw-rw-r--	Sun Sep  2 17:31:39 2018
						listItem.fullQualifiedPath = tokens[0];

						if(tokens[1]=="file"){
							listItem.isFile = true;
						}else{
							listItem.isFile = false;
						}

						listItem.ownership = tokens[2];
						listItem.accessRights = tokens[3];
						listItem.timeStamp = tokens[4];
						
					}

				}

				detailsList.push_back(listItem);
				lineNo ++;
			}
			snapshotFile.close();
		}

		return detailsList;
	}


	void replaceSnapshotFile(string sourcePath,string destinationPath){

		ofstream destinationFile(destinationPath);

		string line;

		if (destinationFile.is_open()){

			ifstream sourceFile(sourcePath);

			if(sourceFile.is_open()){

				while(getline(sourceFile,line)){

					destinationFile << line;
				}

				sourceFile.close();

			}else{

				cout<<"Unable to open source file\n";
			}

			destinationFile.close();
		}
		else{
			cout << "Unable to open destination file\n";	
		} 
	}


	vector<struct compareSnapshot> compareSnapshotFile(string sourcePath,string destinationPath){

		char cwd[PATH_MAX];
		if (getcwd(cwd, sizeof(cwd)) != NULL) 
		{
			writeLog(cwd,1);
		}



		writeLog("Entering into compareSnapshot",1);
		writeLog(sourcePath,1);
		writeLog(destinationPath,1);

		string sp = sourcePath;
		string dp = destinationPath;

		cout<<sourcePath<<"\n";
		cout<<destinationPath<<"\n";

		sourcePath += "/.snapshot";
		destinationPath += "/.snapshot";

		string sourceFileToDelete = sourcePath;

		ifstream sourceSnapshot(sourcePath),destinationSnapshot(destinationPath);
		
		vector<struct snapshotDetails> sourceDetails, destinationDetails;

		vector<struct compareSnapshot> diffList;
		struct compareSnapshot listItem;

		cout<<"File to be opened sourcepath\n";

		sourceDetails = readSnapshot(sourcePath);

		cout<<"File to be opened destinationpath\n";

		destinationDetails = readSnapshot(destinationPath);

		cout<<"File opened successfully\n";

		for(int i=0;i<sourceDetails.size();i++){
			cout<< sourceDetails[i].fullQualifiedPath<<"\n";
		}

		for(int i=0;i<destinationDetails.size();i++){
			cout<< destinationDetails[i].fullQualifiedPath<<"\n";
		}


		// cout<<"prakash\n";

		bool isPresent = false;

		for(int i = 0;i < sourceDetails.size();i++){

			for(int j = 0;j< destinationDetails.size();j++){

				// cout<<"source timestamp: "<<sourceDetails[i].timeStamp<<" \n";
				// cout<<"Dest timestamp: "<<destinationDetails[j].timeStamp<<"\n";

				if(sourceDetails[i].fullQualifiedPath == destinationDetails[j].fullQualifiedPath){

					isPresent = true;

					if(sourceDetails[i].timeStamp != destinationDetails[j].timeStamp){

						// listItem = (struct compareSnapshot*)malloc(sizeof(struct compareSnapshot));
						listItem.details = sourceDetails[i];
						listItem.operationType = "modify";
						diffList.push_back(listItem);
						// free(listItem)

					}
					break;
				}

			}

			if(isPresent == false){

				// listItem = (struct compareSnapshot*)malloc(sizeof(struct compareSnapshot));
				listItem.details = sourceDetails[i];
				listItem.operationType = "create";
				diffList.push_back(listItem);
				// free(listItem);
			}
			isPresent = false;
		}

		for(int i=0;i<destinationDetails.size();i++){
			
			for(int j=0;j<sourceDetails.size();j++){

				if(destinationDetails[i].fullQualifiedPath == sourceDetails[j].fullQualifiedPath){

					isPresent = true;
					break;
				}	
			}

			if(isPresent == false){

				// listItem = (struct compareSnapshot*)malloc(sizeof(struct compareSnapshot));
				listItem.details = destinationDetails[i];
				listItem.operationType = "delete";
				diffList.push_back(listItem);
				// free(listItem);
			}

			isPresent = false;
		}

		cout<<"Difflist size: "<<diffList.size()<<"\n";

		// diffList = removeRedundantEntries(diffList);
		// diffList = sortList(diffList);
		// replaceSnapshotFile(sourcePath,destinationPath);

		cout<<"inside comparesnapshotfile\n";
		
		for(int i=0;i<diffList.size();i++){
			cout<< diffList[i].details.fullQualifiedPath<<" "<<diffList[i].operationType<<"\n";
		}


		CreateSnapShotClass createSnapShotClassObj;
		createSnapShotClassObj.prepareMetadataForSnapshot(toCharArrayFromString(sp),toCharArrayFromString(dp),1);

		chdir(cwd);

		createSnapShotClassObj.CreateSnapshotFile(toCharArrayFromString(sp),toCharArrayFromString(dp));

		chdir(cwd);

		int status = remove(toCharArrayFromString(sourceFileToDelete));
		writeLog("**************************************",1);
		cout<<"**************************************"<<"\n";
		

		writeLog(sourceFileToDelete,1);
		if(status == -1){

			writeLog("////////////////////////////////////",1);	
			// cout<<"Error while deleting .snapshot file in source "<<sourceFileToDelete<<"\n";
		}

		writeLog("**************************************",1);
		cout<<"**************************************"<<"\n";
		return diffList;
	}



	bool runTasks(vector<struct compareSnapshot> diffList){

		string dPath = "",sPath = "";

		writeLog("Entering into runTasks",1);
		writeLog(SRCPATH,1);
		writeLog(DESTPATH,1);

		for(int i = 0;i<diffList.size();i++){

			if(diffList[i].operationType == "create"){

				// cout<<SRCPATH<<"\n";

				sPath = SRCPATH+"/"+diffList[i].details.fullQualifiedPath;
				dPath = DESTPATH+"/"+diffList[i].details.fullQualifiedPath;

				CopyFunctionality copyFunctionalityObj;

				// if (diffList[i].details.fullQualifiedPath.find("/") != std::string::npos) {
				dPath = DESTPATH+"/"+diffList[i].details.fullQualifiedPath;
				cout<<"sPath in create : "<<sPath<<"\n\n";
				cout<<"dPath in create before : "<<dPath<<"\n";
				// dPath = SplitFilename(dPath);
				cout<<"dPath in create after : "<<dPath<<"\n";

				// }					
			
				//cout<<"sPath in create : "<<sPath<<"\n";
				// cout<<"dPath in create : "<<dPath<<"\n";
				// int lastindex=0;
				// for(int i=0;i<sPath.length();i++){
				// 		if(sPath[i]=="/")
				// 			lastindex = i;
				// }
				// string folder = "";
				// folder = sPath.substr(lastindex+1,sPath.length());
				if(diffList[i].details.isFile==false)
				{
					if (mkdir(toCharArrayFromString(dPath),0777) == -1)
                        perror("cant do mkdir in compareSnapshot");	
				}else{
					dPath = SplitFilename(dPath);
				}
				
						perror("++++++++Creating File/Folder ");
						perror("for source path: ");
						perror(toCharArrayFromString(sPath));
						//writeLog("Creating File/Folder for Destination Path : ",1);
						writeLog("for destination path: ",1);
						writeLog(dPath,1);
						copyFunctionalityObj.copy(toCharArrayFromString(sPath),toCharArrayFromString(dPath),1);							




			}else if(diffList[i].operationType == "delete"){

				dPath = DESTPATH+"/"+diffList[i].details.fullQualifiedPath;

				DeleteFunctionality deleteFunctionalityObj;

				cout<<"dPath in delete: "<<dPath<<"\n";

				// char *path = toCharArrayFromString(dPath);
				
				// printf("Path: %s\n", path);

				writeLog("--------Deleting File/Folder for path : ",1);
				//writeLog(sPath,1);
				//writeLog("Creating File/Folder for Destination Path : ",1);
				writeLog(dPath,1);
				deleteFunctionalityObj.removedirectory(toCharArrayFromString(dPath));

			}else if(diffList[i].operationType == "modify"){
				
				sPath = SRCPATH+"/"+diffList[i].details.fullQualifiedPath;
				dPath = DESTPATH+"/"+diffList[i].details.fullQualifiedPath;

				string strFullQualifiedPathDestination = dPath;
				string strFullQualifiedPathSource = sPath;

				CopyFunctionality copyFunctionalityObj;

				// if (diffList[i].details.fullQualifiedPath.find("/") != std::string::npos) {
				dPath = DESTPATH+"/"+diffList[i].details.fullQualifiedPath;
				cout<<"sPath in modify : "<<sPath<<"\n\n";		
				cout<<"dPath in modify before : "<<dPath<<"\n";
				dPath = SplitFilename(dPath);
				cout<<"dPath in modify after : "<<dPath<<"\n";

				//get Extension $$$$$$$$
				//if txt then rsync algo
				//else copy

				/*string extension;
				int lengthOfSPath = sPath.length();
				if(lengthOfSPath>0 && sPath[lengthOfSPath-1]==)
				{
					extension = sPath.substr(lengthOfSPath-4,4);	
				}*/
				bool isTextFile = false;
				if(sPath.substr(sPath.find_last_of(".") + 1) == "txt") 
				{
				    std::cout << "Yes...txt is found $$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
				    isTextFile = true;
				}

				// if(isTextFile == true)
				// {
				// 	PerformModifyOperationForFile(strFullQualifiedPathSource,strFullQualifiedPathDestination);
				// }

				if(isTextFile == true)
				{
					int x = PerformModifyOperationForFile(strFullQualifiedPathSource,strFullQualifiedPathDestination);
					if(x == 0){
						return true;
					}
					return false;
				}

				if(isTextFile==false && diffList[i].details.isFile==true){
					copyFunctionalityObj.copy(toCharArrayFromString(sPath),toCharArrayFromString(dPath),1);
				}							
			}
		}

	}

	
	int PerformModifyOperationForFile(string srcFilePath, string backupFilePath){
		cout << "16---------- Inside PerformModifyOperationForFile() ----------" << endl;
        cout<<"Source File Path is: "<< srcFilePath<<endl;
		cout<<"Backup File Path is: "<< backupFilePath << endl;
		int status;
        int chunkSize = getChunkSizeOfFile(backupFilePath);
        cout << " chunkSize "  << chunkSize << endl;
        if(chunkSize <= 0){
			string copyDest = SplitFilename(backupFilePath);
        	cout<<"ChunkSize is less than or equal to zero thus directly copying the source file "<< srcFilePath << " at destination : "<< copyDest<< endl;
			
        	CopyFunctionality copyFunctionalityObj;
        	copyFunctionalityObj.copy(toCharArrayFromString(srcFilePath),toCharArrayFromString(copyDest),1);
			cout<<"Source file "<<srcFilePath<<" is be copied at destination.."<<endl;
        	return 0;
        }
        Rsync rObj;
		string indexFilePath = rObj.prepareIndexOfBackupFile(backupFilePath,chunkSize);
		
		if(indexFilePath == ""){
			cout<<"Error while creating index file before updation of file "<<backupFilePath<<endl;
			return -1;
		}
		else{
			cout<<"index file successfully created before updation of file "<<backupFilePath<<endl;
		}

		string updateIndexFilePath = rObj.prepareUpdateIndexFile(srcFilePath, indexFilePath, chunkSize);
		if (updateIndexFilePath == ""){
			cout<<"Error while creating .Updateindex file for file "<<srcFilePath<<endl;
			return -1;
		}
		status = rObj.updateDataBackupFile(backupFilePath, updateIndexFilePath, chunkSize);
		if(status == -1){
			cout<<"Error while updating Backup file "<<backupFilePath<<endl;
			return -1;
		}
		else{
			cout<<"Backup file "<<backupFilePath<<" updated successfully"<<endl;
		}
		perror("1111Checking Cursor");
		//Comment below code if you want to see the status of .index and .updateIndex files
		status = remove((char *)indexFilePath.c_str());
		if(status == -1){
			cout<<"Error while deleting .index file for backup file "<<backupFilePath<<endl;
			return -1;
		}
		else{
			cout<<"index file "<<backupFilePath<<" deleted successfully"<<endl;
		}

		status = remove((char *)updateIndexFilePath.c_str());
		if(status == -1){
			cout<<"Error while deleting .updateIndex file for src file "<<srcFilePath<<endl;
			return -1;
		}
		else{
			cout<<"Backup file "<<srcFilePath<<" deleted successfully"<<endl;
		}
		return 0;
	}

};
