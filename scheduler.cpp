#include "compareSnapshot.h"
#define MDSchedulerLogFilePath "./LogFile/SchedulerLog.txt"
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
				vector<string> tC = split(itemContent.lastRunTime,"-");
				int year;int month;int day;int hour; int min;int sec;
				if(tC.size()>=6)
				{
					year=stoi(tC[0]);month=stoi(tC[1]);day=stoi(tC[2]);
					hour=stoi(tC[3]);min=stoi(tC[4]);sec=stoi(tC[5]);
				} 
				std::tm lastModifiedTime = make_tm(year,month,day,hour, min,sec);
				std::tm* lastModifiedTimeptr = &lastModifiedTime;
				std::time_t now = time(0);
				// Convert now to tm struct for local timezone
				std::tm* localtm = localtime(&now);

				std::time_t time2 = std::mktime(lastModifiedTimeptr);
				std::time_t time1 = std::mktime(localtm);

				double portable_difference = std::difftime(time1, time2);
				
				cout << "Current Time" << time1 << endl; 				
				cout << "From File " << time2 << endl;
				cout << portable_difference << endl;

				if(portable_difference>timeInterval)
				{
					cout << "Added : " << itemContent.lastRunTime << endl;
					metadataToProcessForScheduler.push_back(itemContent);
				}
			}
		}
		return metadataToProcessForScheduler;
}

char * getCurrentTime()
{
    std::time_t result = std::time(nullptr);
    return std::asctime(std::localtime(&result));
}

bool writeLog(string Data)
{
	bool success = false;
	if(Data!="")
	{
		std::ofstream out;
 		out.open(MDSchedulerLogFilePath, std::ios::app);
 		string strDate(getCurrentTime());
 		out <<  strDate << ":" << Data << endl;
		out.close();
	}
	return success;
}


bool endsWith(const std::string& s, const std::string& suffix)
{
    return s.size() >= suffix.size() &&
           s.substr(s.size() - suffix.size()) == suffix;
}

std::tm make_tm(int year,int month,int day,int hour, int min,int sec)
{
    std::tm tm = {0};
    tm.tm_year = year - 1900; // years count from 1900
    tm.tm_mon = month - 1;    // months count from January=0
    tm.tm_mday = day;         // days count from 1
    tm.tm_hour = hour; // years count from 1900
    tm.tm_min = min;    // months count from January=0
    tm.tm_sec = sec;         // days count from 1
    return tm;
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


	Scheduler schedulerObj;
	while(true)
	{

		//checking metadata file
		//firing thread for expired timestamp
		
		schedulerObj.writeLog(" Scheduler Started ");
		vector<SnapShotMetaDataInformation> snapShotToProcess;

		snapShotToProcess = schedulerObj.CreateManifest();

		for(int i=0;i<snapShotToProcess.size();i++){

			cout<<"Scheduler: "<<snapShotToProcess[i].sourcePath<<" "<<snapShotToProcess[i].destinationPath<<"\n";
			//thread(schedulerObj.processSnapShot,snapShotToProcess[i].sourcePath,snapShotToProcess[i].destinationPath).detach();	

		}
		
		// unsigned int microseconds = 1200000;
		schedulerObj.writeLog(" Scheduler About to Sleep ");

		sleep(timeInterval);

		schedulerObj.writeLog(" Scheduler Awaked ");
	

	}

	schedulerObj.writeLog(" Scheduler Stopped ");

	return 0;
}