// file: util.h
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: header for general purpose utility functions
//

#include "util.h"
#include <cstdlib>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>


// function to trim the front side of a string so it is size length 
std::string TrimFront(const std::string &str, unsigned length) {
   std::string ret;

   if(str.length() > length) {
      ret = str.substr(str.length() - length, std::string::npos);
   } // end if 

   return ret;
} // end TrimFront

// return the desktop path (windows only)
std::string GetDesktopPath() {
   std::string desktop = std::getenv("USERPROFILE");
   desktop += "\\Desktop";
   return desktop;
} // end GetDesktopPath

// return a string with the substring str1 replaced by str2
std::string ReplaceSubstring(std::string input,
                             const std::string &str1,
                             const std::string &str2) {
   boost::replace_all(input, str1, str2);
   return input;
} // end ReplaceSubstring


// remove leading and training white space
std::string TrimString(const std::string &str) {
   using namespace boost::algorithm;
   return trim_copy(str);
} // end TrimString


// remove all space characters 
std::string RemoveAllSpaces(const std::string &str) {
   using namespace boost::algorithm;
   return erase_all_copy(str," ");
} // end RemoveAllSpaces


// make a vector on delimited tokens
// ref: http://www.boost.org/doc/libs/1_66_0/libs/tokenizer/char_separator.htm
StringVector TokenizeOnDelimiters(const std::string &str,
                                  const std::string &delimiter) {
   std::vector<std::string> ret;

   using namespace boost;

   // use typedefs to keep the lines shorter 
   typedef tokenizer<char_separator<char>> Tokenizer;
   typedef Tokenizer::iterator TokenizerIter;

   char_separator<char> sep(delimiter.c_str());
   Tokenizer tokens(str, sep);

   TokenizerIter iter = tokens.begin();
   for(; iter != tokens.end(); ++iter) {
      ret.push_back(*iter);
   } // end for 

   return ret;
} // end TokenizeOnChar


std::set<char> ConvertStringToSet(const std::string &str) {
   std::set<char> ret;

   for(std::size_t i = 0; i < str.length(); ++i)
      ret.insert(str[i]);

   return ret;
} // end ConvertStringToSet


// ref http://en.cppreference.com/w/cpp/container/set/find
int IsStringInAlphabet(const StringSet &alphabet, const std::string &str) {
   int ret = 0;

   for(std::size_t i = 0; i < str.length(); ++i){

      // assume that the alphabet is a set of single chars 
      std::string aChar;
      aChar = str[i];

      auto pos = alphabet.find(aChar);
      if(pos == alphabet.end()) {
         ret = -1; // not found 
      } // end if 

   } // end 

   return ret;
} // end ConvertStringToSet

// remove comments that start with "//" 
std::string RemoveComments(std::string str) {
   std::string ret;
   
   auto idx = str.find("//");   
   if(idx != std::string::npos) {
      ret = str.erase(idx, std::string::npos);
   }
   else {
      ret = str;
   } // 

   return ret;
} // end RemoveComments


  // get the filename w/o extention from path
  // return 0 success
  // return -1 filename not found 
int GetFilenameFromPath(std::string &path){
   int ret = 0;

   // ref http://www.cplusplus.com/reference/string/string/find_last_of/
   std::string fname = path;
   fname = fname.erase(fname.find_last_of("."), std::string::npos);
   fname = fname.erase(0, fname.find_last_of("/") + 1);
   path = fname;

   return ret;
} // end GetFilenameFromPath



