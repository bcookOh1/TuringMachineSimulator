// file: util.h
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: header for general purpose utility functions
// note: see the .cpp implementation for function comments


#pragma once

#include <string>
#include <vector>
#include <set>
#include "CommonDef.h"


// function to trim the front side of a string so it is size length 
std::string TrimFront(const std::string &str, unsigned length);

// return the desktop path (windows only)
std::string GetDesktopPath();

// return a string with the substring str1 replaced by str2
std::string ReplaceSubstring(std::string input, 
                             const std::string &str1,
                             const std::string &str2);

// remove leading and training white space
std::string TrimString(const std::string &str);

// remove all space characters 
std::string RemoveAllSpaces(const std::string &str);

// make a vector on delimited tokens
StringVector TokenizeOnDelimiters(const std::string &str,
                                  const std::string &delimiter);

// convert a string to a set  
std::set<char> ConvertStringToSet(const std::string &str);

// test if the string in the alphabet
int IsStringInAlphabet(const StringSet &alphabet, const std::string &str);

// remove comments that start with "//" 
std::string RemoveComments(std::string str);

// get the filename w/o extention from path
// return 0 success
// return -1 filename not found 
int GetFilenameFromPath(std::string &path);

