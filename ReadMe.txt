
Chitta : 

ip:  CreateSnapshotFile(sourceDirectoryPath,destinationdirectoryPath)
op:  A text file is written with Details Obtained by GetMetaDataForDirectory function 
Creation of .snapShot File :

	a) Create template in file .snapshot_template
	b) Save the Details of File in .snapShot file

	----------------

ip : GetMetaDataForDirectory(sourcePath)
op : List of Details of Folder and Files 
Get Metdata About Directory(Path)
	recursively 

-------------------
Nitish : 

ip:  CreateSnapshotMetadataFile(sourceDirectoryPath,destinationdirectoryPath)
op:  A text file is written with Details about new Snapshot Created  
creation of .snapsot__metadata__ at specified location (/home/userfolder)
	Number of snapshot details  

ip:  removefromSnapshotMetadataFile(sourceDirectoryPath,destinationdirectoryPath)
op:  snapsot__metadata__ Details willl remove particular snapshot line from file   
remove of line from .snapsot__metadata__ at specified location (/home/userfolder)  

CheckSnapShotExistForFile(sourceDirectoryPath,destinationdirectoryPath)

Done :

Comands Created inside executable need to process: 

startSnapShotEngine -> start process : jha to give program which can run in background 

CreateSnapShot SourcePath DestinationPath : working 

restoreSnapShot SourcePath : not yet

stopSnashot sourcePath : working


-------------------
Jha : 

ip: comaresnapshotfile(sourcePathSnapshot,destinationPathSnapshot)
op: provide data structure with difference between them 
vector<CompareSnapShotDS> comparesnapshotfile(sourceFile,destinationFile)

PerformOperation()
{
	for each iterated in vector<CompareSnapShotDS>
	delete
	modify
	create-> copy 
}

to remove redundant file copy and delete operations from list

-------------------
Suraj :  

ip: replaceModifiedData(sourcePath,destinationPath)
op: ouput success/reject 
rsync Implemntation (sourcePath,destinationPath)

a)if file not exist copy
b)if file exist then apply rsync on it -> destinationpath

-------------------

struct SnapShotMetaDataInformation
{
	string sourcePath;
	string destinationPath;
	string dateTimeStamp;
}

struct SnapShotDetails
{
	string fullQualifiedPath;
	string ownership;
	string TimeStamp;
	string accessRights;
}

strcut enum OperationType
{
	Create;
	Delete;
	Modify;
}

struct CompareSnapShotDS
{
	struct SnapShotDetails;
	string newTimeStamp;
	string oldTimeStamp;
	string OperationType;
}

------------------------------
main()
{
	Driver Function()	
}

-----------------------------

CopyFunctional(sourcePath,destinationPath)
	Recusrsive Copy 

delete(path)
	Recusrsive Delete 

------------------------------

Comands : 

startSnapShotEngine -> start process

CreateSnapShot SourcePath DestinationPath

restoreSnapShot SourcePath

stopSnashot sourcePath 



------------------------------------Execution Sample For Modules ---------------

Command on terminal : 

createSnapShot /home/prakashjha/os/workarea/OS_Snapshot/repo /home/prakashjha/os/workarea/OS_Snapshot/repoSnapShot
removeSnapShot /home/prakashjha/os/workarea/OS_Snapshot/repo 

./createSnapshot 
/home/prakashjha/os/workarea/OS_Snapshot/repo                                                     
/home/prakashjha/os/workarea/OS_Snapshot/repoSnapShot


/////////////////////////////////////////////////////////////////////////

createSnapShot {sourcePath} {destinationPath}

removeSnapShot {sourcePath}

restoreSnapShot {sourcePath}

startScheduler

stopScheduler