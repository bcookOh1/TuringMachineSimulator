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


class GvToImage {
public:

   GvToImage();
   ~GvToImage();

   string GetGvFile() {return _gvFile;}
   vector<string> GetStates() { return _tmStates; }
   size_t GetImageCount() { return _imgFiles.size(); }
   string GetErrorString() { return _errorStr; }

   int ConvertGv(const string &gvFile);
   int GetImgFromStateName(const string &stateName, string &imgFile);
   int GetStateNameAndImageFromIndex(size_t idx, string &stateName, string &imgFile);

private:

   string _gvFile;
   vector<string> _tmStates;
   map<string,string> _imgFiles;
   string _errorStr;

   int ReadStatesFromGv();
   int MakeSvgForState(const string &stateName, const string &svgFile, string &newsvg);
   int ConvertSvgToImage(const std::string &svgFile, string &imgFile);

}; // end class

