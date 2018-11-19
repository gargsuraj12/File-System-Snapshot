#include<bits/stdc++.h>

using namespace std;

string SRCPATH = "";
string DESTPATH = "";

struct snapshotDetails{

	string fullQualifiedPath;
	string ownership;
	string timeStamp;
	string accessRights;
};

struct compareSnapshotDS{

	struct snapshotDetails details;
	string newTimeStamp;
	string oldTimeStamp;
	string operationType;

};


vector<string> split(char *phrase, string delimiter){
    vector<string> list;
    string s = string(phrase);
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        list.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    list.push_back(s);
    return list;
}


vector<struct snapshotDetails> readSnapshot(string path){

	ifstream snapshotFile(path);

	vector<struct snapshotDetails> detailsList;

	struct snapshotDetails listItem;

	string line,sPath,dPath;
	
	int lineNo = 1;

	if(snapshotFile.is_open()){

		while(getline(snapshotFile,line)){

			if(lineNo==1){
				
				line = sPath;
				SRCPATH = sPath;
			
			}else if(lineNo == 2){

				line = dPath;
				DESTPATH = dPath;

			}else if(lineNo >= 4){

				char *fileLine = new char [line.length()+1];
				strcpy(fileLine, line.c_str());
				
				vector<string> tokens = split(fileLine,"\t");
				
				listItem.fullQualifiedPath = sPath +"/"+tokens[0];
				listItem.ownership = tokens[1];
				listItem.timeStamp = tokens[2];
				listItem.accessRights = tokens[3];

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


vector<struct compareSnapshotDS> compareSnapshotFile(string sourcePath,string destinationPath){

	ifstream sourceSnapshot(sourcePath),destinationSnapshot(destinationPath);
	
	vector<struct snapshotDetails> sourceDetails, destinationDetails;

	vector<struct compareSnapshotDS> diffList;
	struct compareSnapshotDS listItem;

	sourceDetails = readSnapshot(sourcePath);

	destinationDetails = readSnapshot(destinationPath);

	bool isPresent = false;

	for(int i = 0;i < sourceDetails.size();i++){

		for(int j = 0;j< destinationDetails.size();j++){

			if(sourceDetails[i].fullQualifiedPath == destinationDetails[j].fullQualifiedPath){

				isPresent = true;

				if(sourceDetails[i].timeStamp != destinationDetails[j].timeStamp){

					// listItem = (struct compareSnapshotDS*)malloc(sizeof(struct compareSnapshotDS));
					listItem.details = sourceDetails[i];
					listItem.operationType = "modify";
					diffList.push_back(listItem);
					// free(listItem)

				}
				break;
			}

		}

		if(isPresent == false){

			// listItem = (struct compareSnapshotDS*)malloc(sizeof(struct compareSnapshotDS));
			listItem.details = sourceDetails[i];
			listItem.operationType = "create";
			diffList.push_back(listItem);
			// free(listItem);
		}
		isPresent = false;
	}

	for(int i=0;i<destinationDetails.size();i++){
		
		for(int j=0;i<sourceDetails.size();j++){

			if(destinationDetails[i].fullQualifiedPath == sourceDetails[j].fullQualifiedPath){

				isPresent = true;
				break;
			}	
		}

		if(isPresent == false){

			// listItem = (struct compareSnapshotDS*)malloc(sizeof(struct compareSnapshotDS));
			listItem.details = sourceDetails[i];
			listItem.operationType = "delete";
			diffList.push_back(listItem);
			// free(listItem);
		}

		isPresent = false;
	}

	replaceSnapshotFile(sourcePath,destinationPath);

	return diffList;
}


bool runTasks(vector<struct compareSnapshotDS> diffList){

	string dPath = "",sPath = "";

	for(int i = 0;i<diffList.size();i++){

		if(diffList[i].operationType == "create"){

			sPath = SRCPATH+"/"+diffList[i].details.fullQualifiedPath;
			dPath = DESTPATH+"/"+diffList[i].details.fullQualifiedPath;
			createNewFile(sPath,dPath);

		}else if(diffList[i].operationType == "delete"){

			dPath = DESTPATH+"/"+diffList[i].details.fullQualifiedPath;
			deleteFile(sPath,dPath);

		}else if(diffList[i].operationType == "modify"){
			
			sPath = SRCPATH+"/"+diffList[i].details.fullQualifiedPath;
			dPath = DESTPATH+"/"+diffList[i].details.fullQualifiedPath;
			updateFile(sPath,dPath);

		}
	}

}


int main(){

	vector<compareSnapshotDS> diffList = compareSnapshotFile(sourcePath,destinationPath);

	bool flag = runTasks(diffList);

	return 0;

}