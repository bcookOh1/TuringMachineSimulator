// file: TuringMachineFactory.cpp
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: implementation for the factory class that reads 
// the definition and builds a TuringMachne class.  
// ReadinDefinitionFile() uses a simple top down line by line 
// parse of the defintion file. The class uses the boost 
// xpressive library to verify and parse each line.
//


#include "TuringMachineFactory.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <utility>
#include <algorithm>
#include "boost\lexical_cast.hpp"


TuringMachineFactory::TuringMachineFactory() {
   _tm = nullptr;
   _readingTransitionTable = false;
} // end ctor 


TuringMachineFactory::~TuringMachineFactory() {
   _alphabet.clear();
} // end dtor 


// file read reference: http://www.cplusplus.com
int TuringMachineFactory::ReadinDefinitionFile(const std::string &definitionfile) {
   int result = 0;
   int lineNumber = 0;

   std::ifstream infile(definitionfile.c_str()); // open the file

   if(infile.fail()) {
      _errorString = "file open failed";
      return -1;
   } // end if 

   // make a new turing machine class 
   if(_tm != nullptr)
      delete _tm;

   // make new and initialize 
   _tm = new TuringMachine(); // MainWind will own this and delete on close 
   _alphabet.clear();

   // flag to know when we are reading the transition table data
   _readingTransitionTable = false;

   std::string line;
   while(infile.good() && result == 0) {
      std::getline(infile, line, '\n');
      lineNumber++;

      // remove comments 
      line = RemoveComments(line);

      // remove white space
      line = TrimString(line);

      // skip blank lines 
      if(line.empty() != true) {

         // line pass to the factory  
         result = Factory(line, lineNumber);
      } // end if 

   } // end while 

   infile.close();

   return result;
} // end ReadinDefinitionFile


// return 0 success
// return -1 unknown line 
int TuringMachineFactory::Factory(const std::string &line, int lineNumber) {

   // for boost lexical_cast
   using namespace boost;

   int result = 0;

   // xpressive string token match container   
   smatch tokens;

   // do a top down parse of the defintion file 
   if(regex_search(line, tokens, Sre_Configuration)) {
      _tm->SetConfiguration(tokens[s1].str());
   } 
   else if(regex_search(line, tokens, Sre_Description)) {
      _tm->SetDescription(tokens[s1].str());
   } 
   else if(regex_search(line, tokens, Sre_Input_Example)) {
      _tm->SetInputExample(tokens[s1].str());
   } 
   else if(regex_search(line, tokens, Sre_Start_State)) {
      _tm->SetStartState(tokens[s1].str());
   } 
   else if(regex_search(line, tokens, Sre_Two_Way_Boundary)) {
      _tm->SetTwoWayTapeBoundary(tokens[s1].str());
   }
   else if(regex_search(line, tokens, Sre_Final_States)) {

      // a set container of final states 
      StringSet finalStates;
      result = GroupToStringSet(tokens[s1].str(), finalStates);
      if(result == 0) {
         _tm->SetFinalState(finalStates);
      } else {
         _errorString = "final states parsing error";
      } // end if 

   }
   else if(regex_search(line, tokens, Sre_Tape_Symbols)) {

      // a set container for tape symbols 
      StringSet tapeSymbols;
      result = GroupToStringSet(tokens[s1].str(), tapeSymbols);
      if(result == 0) {
         _tm->SetTapeSymbols(tapeSymbols);
      } 
      else {
         _errorString = "tape symbols parsing error";
      } // end if 

   }
   else if(regex_search(line, tokens, Sre_Alphabet)) {

      // a set container for alphabet
      StringSet alphabet;
      result = GroupToStringSet(tokens[s1].str(), alphabet);
      if(result == 0) {
         _tm->SetAlphabet(alphabet);
      }
      else {
         _errorString = "alphabet parsing error";
      } // end if 

   }
   else if(TRANSITION_FUNCTION_TABLE_BEGIN_LABEL == line) {
      // read and ignore this 
   }
   else if(TRANSITION_FUNCTION_TABLE_END_LABEL == line) {
      _readingTransitionTable = false;
   } 
   else if(regex_search(line, tokens, Sre_Table_Header)) {
      _readingTransitionTable = true;
      _tm->SetDefinitionLine(line);

      StringVector headerRow;
      result = FileHeaderToTransitionHeader(line, headerRow);
      if(result != 0) {
         _errorString = "table header, line: " + lexical_cast<std::string>(lineNumber) +
            " format error";
      } else {
         _alphabet = headerRow;
         _tm->SetTransitionTableHeader(headerRow);
      } // end if 
   }
   else if(true == _readingTransitionTable) { // make this last in if 
      _tm->SetDefinitionLine(line);

      TransitionRow transitionRow;
      result = FileRowToTransitionRow(line, _alphabet, transitionRow);
      if(result != 0) {
         _errorString = "table row, line: " + lexical_cast<std::string>(lineNumber) +
                         " format error";
      }
      else {
         _tm->AddTransitionTableRow(transitionRow);
      } // end if 

   } 
   else {
      _errorString = "file line " + lexical_cast<std::string>(lineNumber) + 
                     " not defined";
      result = -1;
   } // end if 

   return result;
} // end TuringMachineFactory


int FileHeaderToTransitionHeader(std::string line, 
                                 StringVector &headerRow) {
   int ret = 0;

   line = RemoveAllSpaces(line);
   StringVector columns = TokenizeOnDelimiters(line, CELL_DELIMITER);

   // remove the "delta" 
   if(columns.size() > 0 && columns[0] == DELTA) {
      columns.erase(columns.begin());
   } // end if 

   headerRow = columns;

   return ret;
} // end FileHeaderToTransitionHeader


// pre: FileRowToTransitionRow() must be run first so _alphabet is current 
int FileRowToTransitionRow(std::string line, 
                           const StringVector &alphabet,
                           TransitionRow &transitionRow) {
   int ret = 0;

   line = RemoveAllSpaces(line);
   StringVector columns = TokenizeOnDelimiters(line, CELL_DELIMITER); 

   // test that the alphabet count and column count match  
   if(alphabet.size() + 1 != columns.size()) {
      assert(false);
   } // end if 

   // define a pair to insert into a row map    
   std::pair<std::string, Transition> alphabetSymbolAndCell;

   // define a row map of alphabet symbols and tansitions table cells 
   std::map<std::string, Transition> row;

   for(std::size_t i = 1; i < columns.size(); ++i) {
      auto cell = TokenizeOnDelimiters(columns[i],",");

      // not add is a cell if "empty" (item count != 3)
      if(cell.size() == TABLE_CELL_ITEM_COUNT) {

         Transition t(cell[0],cell[1],cell[2]);
         alphabetSymbolAndCell = std::make_pair(alphabet[i-1], t);
         row.insert(alphabetSymbolAndCell);

      } // end if 
   } // end for 

   // add the state and row to the transitionRow pair 
   transitionRow.first = columns[0]; // state 
   transitionRow.second = row;       // transitions w/ alphabet 

   return ret;
} // end FileRowToTransitionRow


int GroupToStringSet(std::string line, StringSet &stringSet) {
   int ret = 0;

   line = RemoveAllSpaces(line);
   StringVector stringVector = TokenizeOnDelimiters(line, ",");
   stringSet.clear();

   // add to items to stringSet 
   try {
      // ref https://codeyarns.com/2010/07/16/c-stl-inserting-vector-into-set/
      std::copy(stringVector.begin(), stringVector.end(),
                std::inserter(stringSet, stringSet.end()));
   }
   catch(std::exception&) {
      assert(false);
      ret = -1;
   } // end 

   return ret;
} // end GroupToStringSet


