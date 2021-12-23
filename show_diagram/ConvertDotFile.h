
#pragma once


#include <gvc.h>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "CommonDefs.h"


using namespace std;
using namespace boost::algorithm;


class ConvertDotFile {
public:
   ConvertDotFile();
   ~ConvertDotFile();

   // ret = 0 success
   // ret = -1 create gv context failed
   int Init();

   // ret = 0 success
   // ret = -1 invalid file name
   // ret = -2 dot file readin error
   // ret = -3 layout error
   // ret = -4 render error
   int DotFileToSvg(const string &gvFile, string &svgFile);
   string GetErrorString() { return _errorStr; }

private:

   GVC_t *_gvc;
   string _errorStr;

}; // end class

