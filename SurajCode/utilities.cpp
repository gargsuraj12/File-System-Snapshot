#include "utilities.h"

vector<string> splitLine(string line, string delimiter){
	vector<string> splitV;
	string token;
	int index = line.find(delimiter);
	while(index != -1){
		token = line.substr(0, index);
		splitV.push_back(token);
		line.erase(0, index+1);
		index = line.find(delimiter);
	}
	if(!line.empty()){
		splitV.push_back(line);
	}
	return splitV;
}

void printVec(vector<string> vec){
	for(auto it : vec){
		cout<<it<<endl;
	}
}