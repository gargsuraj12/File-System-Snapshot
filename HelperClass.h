
using namespace std;

char* toCharArrayFromString(string str){

    // char charStr[str.length()+1];

    char *charStr;

    charStr = (char*)malloc(str.length()+1);

    strcpy(charStr,str.c_str());
    return charStr;
}

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

string SplitFilename (string str)
{
  //std::cout << "Splitting: " << str << '\n';
  // cout<<"inside helper input: "<<str<<"\n";

  size_t found = str.find_last_of('/');
  str = str.substr(0,found);

  // cout<<"inside helper output: "<<str<<"\n";
  return str;
  //std::cout << " path: " << str.substr(0,found) << '\n';
  //std::cout << " file: " << str.substr(found+1) << '\n';
}