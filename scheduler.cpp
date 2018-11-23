#include "compareSnapshot.h"

using namespace std;

class Scheduler{

public: 

void processSnapShot(string sourcePath,string destinationPath){

	
	SyncData syncDataObj;
	
	vector<compareSnapshot> diffList = syncDataObj.compareSnapshotFile(sourcePath,destinationPath);
	syncDataObj.runTasks(diffList);

}

vector<struct SnapShotMetaDataInformation> CreateManifest()
{
		vector<struct SnapShotMetaDataInformation> metadataContent = ProcessMetadataFileIntoCollection();
		vector<struct SnapShotMetaDataInformation> metadataToProcessForScheduler;
		if(metadataContent.size()>0)
		{
			for(auto itemContent : metadataContent)
			{
				//if(itemContent.lastRunTime>0)
				{
					cout << itemContent.lastRunTime << endl;
				}
				metadataToProcessForScheduler.push_back(itemContent);
			}
		}
		return metadataToProcessForScheduler;
}

bool endsWith(const std::string& s, const std::string& suffix)
{
    return s.size() >= suffix.size() &&
           s.substr(s.size() - suffix.size()) == suffix;
}

std::vector<std::string> split(const std::string& s, const std::string& delimiter, const bool& removeEmptyEntries = false)
{
    std::vector<std::string> tokens;

    for (size_t start = 0, end; start < s.length(); start = end + delimiter.length())
    {
         size_t position = s.find(delimiter, start);
         end = position != string::npos ? position : s.length();

         std::string token = s.substr(start, end - start);
         if (!removeEmptyEntries || !token.empty())
         {
             tokens.push_back(token);
         }
    }

    if (!removeEmptyEntries &&
        (s.empty() || endsWith(s, delimiter)))
    {
        tokens.push_back("");
    }

    return tokens;
}


vector<struct SnapShotMetaDataInformation> ProcessMetadataFileIntoCollection()
{
	std::string line;
	std::ifstream file(MDPath);
	vector<string> tokens;
	bool firstLine=false;
	vector<struct SnapShotMetaDataInformation> AllDataOfFile;
	while (getline(file,line))
	{
		if(firstLine==false)
		{
			firstLine=true;
			continue;
		}

 		tokens = split(line,"\t");	
		if(tokens.size()>0)
		{
			struct SnapShotMetaDataInformation information;
			information.sourcePath=tokens[0];
			information.destinationPath=tokens[1];
			information.creationTimeStamp=tokens[2];
			information.lastRunTime=tokens[3];
			AllDataOfFile.push_back(information);
		}
	}
	return AllDataOfFile;
}
};

int main(){

	// while(true)
	{

		//checking metadata file
		//firing thread for expired timestamp
		
		Scheduler schedulerObj;

		vector<SnapShotMetaDataInformation> snapShotToProcess;

		snapShotToProcess = schedulerObj.CreateManifest();

		for(int i=0;i<snapShotToProcess.size();i++){

			cout<<"Scheduler: "<<snapShotToProcess[i].sourcePath<<" "<<snapShotToProcess[i].destinationPath<<"\n";
			//thread(processSnapShot,snapShotToProcess[i].sourcePath,snapShotToProcess[i].destinationPath).detach();	

		}
		
		// unsigned int microseconds = 1200000;
		usleep(timeInterval);
	

	}

	return 0;
}