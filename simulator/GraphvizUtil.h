
#pragma once


#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>

using namespace std;
namespace fs = std::filesystem;
using std::begin;
using std::end;

// friend function to 
int WriteGraphvizDotFile(TuringMachine *tm, std::string tmFilename, std::string &gvFullPath);
