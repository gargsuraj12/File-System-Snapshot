#include "library.h"
#include "HelperClass.h"
#include "DeleteFile.h"
#include "initialCopy.h"

using namespace std;


class SyncData{

public:

	string SRCPATH = "";
	string DESTPATH = "";


	vector<struct snapshotDetails> readSnapshot(string path){

		ifstream snapshotFile(path);

		vector<struct snapshotDetails> detailsList;

		struct snapshotDetails listItem;

		string line,sPath,dPath;
		
		int lineNo = 1;

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

		ifstream sourceSnapshot(sourcePath),destinationSnapshot(destinationPath);
		
		vector<struct snapshotDetails> sourceDetails, destinationDetails;

		vector<struct compareSnapshot> diffList;
		struct compareSnapshot listItem;

		sourceDetails = readSnapshot(sourcePath);

		destinationDetails = readSnapshot(destinationPath);

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

		//replaceSnapshotFile(sourcePath,destinationPath);

		return diffList;
	}



	bool runTasks(vector<struct compareSnapshot> diffList){

		string dPath = "",sPath = "";

		for(int i = 0;i<diffList.size();i++){

			if(diffList[i].operationType == "create"){

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
				if(diffList[i].details.isFile==false){
					if (mkdir(toCharArrayFromString(dPath),0777) == -1)
                        perror("cant do mkdir in compareSnapshot");	
				}else{
					dPath = SplitFilename(dPath);
				}
				
				copyFunctionalityObj.copy(toCharArrayFromString(sPath),toCharArrayFromString(dPath),1);							




			}else if(diffList[i].operationType == "delete"){

				dPath = DESTPATH+"/"+diffList[i].details.fullQualifiedPath;

				DeleteFunctionality deleteFunctionalityObj;

				cout<<"dPath in delete: "<<dPath<<"\n";

				// char *path = toCharArrayFromString(dPath);
				
				// printf("Path: %s\n", path);

				deleteFunctionalityObj.removedirectory(toCharArrayFromString(dPath));

			}else if(diffList[i].operationType == "modify"){
				
				sPath = SRCPATH+"/"+diffList[i].details.fullQualifiedPath;
				dPath = DESTPATH+"/"+diffList[i].details.fullQualifiedPath;

				CopyFunctionality copyFunctionalityObj;

				// if (diffList[i].details.fullQualifiedPath.find("/") != std::string::npos) {
				dPath = DESTPATH+"/"+diffList[i].details.fullQualifiedPath;
				cout<<"sPath in modify : "<<sPath<<"\n\n";		
				cout<<"dPath in modify before : "<<dPath<<"\n";
				dPath = SplitFilename(dPath);
				cout<<"dPath in modify after : "<<dPath<<"\n";

				// }					
			
				// cout<<"sPath in modify : "<<sPath<<"\n";
				// cout<<"dPath in modify : "<<dPath<<"\n";

				copyFunctionalityObj.copy(toCharArrayFromString(sPath),toCharArrayFromString(dPath),1);							
			}
		}

	}

};