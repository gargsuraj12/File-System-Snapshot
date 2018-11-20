#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

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

int main(){
    int n;
    string data;
    cout<<"Enter the Number of records to be written to the file: "<<endl;
    cin>>n;
    cin.ignore();
    ofstream fwrite;
    fwrite.open("records.dat", ios::binary);
    for(int i=1; i<=n; i++){
        cout<<"Enter name: ";
        getline(cin, data);
        fwrite<<data<<"|";
        cout<<"Enter age: ";
        getline(cin, data);
        fwrite<<data<<"\n";
    }
    fwrite.close();

    ifstream fread;
    fread.open("records.dat", ios::binary);
    vector<string> line;
    getline(fread, data);
    while(fread){
        line = splitLine(data, "|");
        cout<<line[0]<<"\t"<<line[1]<<endl;
        getline(fread, data);
    }
    fread.close();
    return 0;
}