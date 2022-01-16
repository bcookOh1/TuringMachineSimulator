// file: TuringMachine.h
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: header definition for the Turing Machine simulator.
// This is the main class that implements the Turing Machine simulator.
// At the heart, it is just a lookup table for the transitions based 
// on a state (a string) and a symbol (string). The look up is a 
// standard template library map within a map, it makes the loading 
// transitions and doing the look up very simple. The tape itself
// is a standard template library list (doubly linked list) so it 
// can expand left or right as needed. TransitionStep() is the main 
// function that "steps" the simulator.   
// note: see the .cpp implementation for function comments


#pragma once

#include "CommonDef.h"
#include "Transition.h"
#include <string>
#include <vector>
#include <set>
#include <list>
#include <stack>
#include <tuple>


class TuringMachine {
private:

   TapeList _tape;

   // used to update the GUI 
   std::string _currentState;
   std::string _currentSymbol;
   std::string _currentDirection; 

   int _currentPositionIdx;

   TapeIter _headPositionIter;
   TmStatus _status;

   // definition file configuration 
   std::string _configuration;
   std::string _description;
   std::string _inputString;
   std::string _inputExample;
   std::string _startState;
   std::string _twoWayBoundary;
   StringSet _tapeSymbols;
   StringSet _alphabet;
   StringSet _finalStates;
   StringVector _headerRow;
   StringVector _definition;
   TmTransitionTable _transitionTable;
   TmConfiguration _configurationType;

   // used in GetFirst() GetNext() functions used 
   // in mainwind tapesetup only 
   TapeIter _tapeIter;

   // 01-14-2022
   std::stack<Transition> _history;

   bool IsStateFinal(std::string state);
   int MoveHeadRight();
   int MoveHeadLeft();

public:
   TuringMachine();
   ~TuringMachine();

   int SetTransitionTableHeader(const StringVector &headerRow);
   void SetDefinitionLine(const std::string &line) { _definition.push_back(line);}
   int AddTransitionTableRow(const TransitionRow &transitionRow);
   void ClearTransitionTable();

   void SetInputString(std::string in) {_inputString = in;}
   std::string GetInputString(){return _inputString;}
   int Initialize();
   
   int TransitionStep();
   int GetTapeHeadPosition();
   Transition GetCurrentTransition();

   std::string GetState();
   std::string GetComputation();
   StringVector GetDefinition();

   // get set functions for the definition file data  
   std::string GetConfiguration() {return _configuration;}
   TmConfiguration GetConfigurationType() {return _configurationType;}
   int SetConfiguration(std::string configuration);

   std::string GetDescription() {return _description;}
   void SetDescription(std::string description) { _description = description; }

   std::string GetInputExample() {return _inputExample;}
   void SetInputExample(std::string inputExample) { _inputExample = inputExample; }

   std::string GetStartState() {return _startState;}
   void SetStartState(std::string startState) { _startState = startState; }

   StringSet GetFinalState() { return _finalStates; }
   void SetFinalState(StringSet finalStates) { _finalStates = finalStates; }

   StringSet GetAlphabet() { return _alphabet; }
   void SetAlphabet(StringSet alphabet) { _alphabet = alphabet; }

   StringSet GetTapeSymbols() { return _tapeSymbols; }
   void SetTapeSymbols(StringSet tapeSymbols) { _tapeSymbols = tapeSymbols; }

   std::string GetTwoWayTapeBoundary() { return _twoWayBoundary; }
   void SetTwoWayTapeBoundary(std::string twoWayBoundary) { _twoWayBoundary = twoWayBoundary; }

   // return the state as the TmStatus enum
   TmStatus GetTmStatus() {return _status;}

   // get the tape size 
   int GetTapeSize() { return static_cast<int>(_tape.size()); }

   // used as a pair (first/next paradigm) to get tapeData  
   bool GetFirstTapePosition(TapeData &tapeData);
   bool GetNextTapePosition(TapeData &tapeData);

   // make a graphviz doc file 
   int WriteGraphvizDotFile(std::string tmFilename, std::string &gvFullPath);

   // 01-14-2022
   int GetHistorySize() {return static_cast<int>(_history.size());}
   int StepBack(Transition &transition);

   void PrintHeadPosition() {
      std::cout << "head position: (" << std::get<0>(*_headPositionIter) << ", " << std::get<1>(*_headPositionIter) << ")\n";
   } // PrintHeadPosition

}; // end TuringMachine



