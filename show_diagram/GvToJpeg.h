#pragma once

#include "pugixml.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <tuple>
#include <iterator> 
#include "CommonDefs.h"
#include "ConvertDotFile.h"

using namespace std;
using namespace boost::algorithm;


class GvToJpeg {
public:

   GvToJpeg();
   ~GvToJpeg();

   string GetGvFile() {return _gvFile;}
   vector<string> GetStates() { return _tmStates; }
   size_t GetImageCount() { return _jpgFiles.size(); }
   string GetErrorString() { return _errorStr; }

   int ConvertGv(const string &gvFile);
   int GetJpgFromStateName(const string &stateName, string &jpgFile);
   int GetStateNameAndJpgFromIndex(size_t idx, string &stateName, string &jpgFile);

private:

   string _gvFile;
   // string _imgFolder;
   vector<string> _tmStates;
   map<string,string> _jpgFiles;
   string _errorStr;

   int ReadStatesFromGv();
   int MakeSvgForState(const string &stateName, const string &svgFile, string &newsvg);
   int ConvertSvgToJpg(const std::string &svgFile, string &jpgFile);

}; // end class

