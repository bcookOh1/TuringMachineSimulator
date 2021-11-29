// file: TuringMachineFactory.h
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: header defintion for the factory class that reads 
// the definition and builds a TuringMachne class. The TuringMachineFactory
// has a life time during the file readin only. 
// Note that the TuringMachine *_tm is created by the MainWind "owns" it
// and will delete it as needed.
// note: see the .cpp implementation for function comments


#pragma once

#include "CommonDef.h"
#include "util.h"
#include "TuringMachine.h"
#include "Transition.h"
#include <string>


class TuringMachineFactory {
private: 

   TuringMachine *_tm;
   bool _readingTransitionTable;
   std::string _errorString;
   StringVector _alphabet;

   int Factory(const std::string &line, int lineNumber);

public:
   TuringMachineFactory();
   ~TuringMachineFactory();

   int ReadinDefinitionFile(const std::string &definitionfile);
   std::string GetErrorString(){ return _errorString; }
   TuringMachine *GetTuringMachine() { return _tm; }

}; // end class

// associated non-class functions 
int FileHeaderToTransitionHeader(std::string line, StringVector &headerRow);
int FileRowToTransitionRow(std::string line, 
                           const StringVector &alphabet, 
                           TransitionRow &transitionRow);
int GroupToStringSet(std::string line, StringSet &stringSet);

