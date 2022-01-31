
#pragma once

#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>


using namespace std;
namespace fs = std::filesystem;

class TuringMachine;

// friend function to TuringMachine class
int WriteGraphvizDotFile(TuringMachine *tm, string tmFilename, string &gvFullPath);
