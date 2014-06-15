#pragma once

// basic file operations
#include <iostream>
#include <fstream>
#include <string>
#include <map>
using namespace std;

map<string, string> initCfg();
string trim(string const& source, char const* delims = " \t\r\n");

map<string, string> initCfg(){

    // reading a text file
    map<string, string> cfgMap;
    string name;
    string value;
    int posEqual;


    string line;
    ifstream myfile ("config.cfg");
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            if (line[0] == '#') {
		continue;
            }

            posEqual = line.find('=');
            name = trim(line.substr(0,posEqual));
            value = trim(line.substr(posEqual+1));

            cfgMap.insert(pair<string,string>(name, value));

        }
        myfile.close();
    }

    else cout << "Unable to open file"; 

    return cfgMap;
}

string trim(string const& source, char const* delims) {
    string result(source);
    size_type index = result.find_last_not_of(delims);
    if(index != string::npos)
        result.erase(++index);

    index = result.find_first_not_of(delims);
    if(index != string::npos)
        result.erase(0, index);
    else
        result.erase();
    return result;
}