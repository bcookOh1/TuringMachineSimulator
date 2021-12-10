// file: TuringMachine.cpp
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: implementation for the Turing Machine simulator.
// the heart of the class is the TmTransitionTable container which is  
// lookup table for the transitions based on the state and the symbol 
// tape.


#include "TuringMachine.h"
#include <boost/filesystem.hpp>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <set>


// constructor 
TuringMachine::TuringMachine() {
   _status = TmStatus::Uninitialized;
   _currentState = "";
   _currentSymbol = "";
   _currentDirection = TAPE_RIGHT_SYMBOL;
   _currentPositionIdx = 0;
   _configurationType = TmConfiguration::Standard;
   _twoWayBoundary = TWO_WAY_LEFT_BOUNDARY;
} // end ctor


// destructor  
TuringMachine::~TuringMachine() {
} // end dtor


// return 0 success 
int TuringMachine::SetTransitionTableHeader(const StringVector &headerRow) {
   int ret = 0;
   _headerRow = headerRow;
   return ret;
} // end SetTransitionTableHeader


// return 0 success 
// return 1 duplicate ignored  
int TuringMachine::AddTransitionTableRow(const TransitionRow &transitionRow) {
   int ret = 0;

   // ignore duplicates 
   auto result = _transitionTable.insert(transitionRow);

   // return 1 if a duplicate was ignored
   if(result.second == false){
      ret = 1;
   } // end if  

   return ret;
} // end AddTransitionTableRow


// clear the table and the definition string vector 
void TuringMachine::ClearTransitionTable() {
   _transitionTable.clear();
   _definition.clear();
   return;
} // end ClearTransitionTable


// from the param string set for CONFIGURATION_STANDARD or 
// CONFIGURATION_TWO_WAY configuration 
int TuringMachine::SetConfiguration(std::string configuration) {
   int ret = 0;

   // set the configurationType
   if(configuration == CONFIGURATION_TWO_WAY) {
      _configurationType = TmConfiguration::TwoWay;
   } 
   else if(configuration == CONFIGURATION_STANDARD) {
      _configurationType = TmConfiguration::Standard;
   }
   else {
      assert(false); // should not happen so assert() 
   } // end if 

   _configuration = configuration;

   return ret;
} // end SetConfiguration


// initialize the tape with the _inputString string 
// a "B" is added before and after, ex: BaabbccB 
int TuringMachine::Initialize() {

   _tape.clear();
   
   std::string symbol = TAPE_BLANK_SYMBOL;
   int index = 0;

   TapeData td = std::make_tuple(symbol, index);
   _tape.push_back(td);

   // set the initial state and position
   _currentState = _startState;
   _headPositionIter = _tape.begin();
   _currentPositionIdx = index++;

   // setup the tape 
   for(std::size_t i = 0; i < _inputString.length(); ++i) {
      symbol = _inputString[i];
      td = std::make_tuple(symbol, index++);
      _tape.push_back(td);
   } // end for 

   symbol = TAPE_BLANK_SYMBOL;
   td = std::make_tuple(symbol, index);
   _tape.push_back(td);

   _status = TmStatus::Initialized;

   return 0;
} // end Initialize


// MoveHeadRight move the tape head and add a B if 
// the front or back is not already a B
// return 0 success, doesn't fail  
int TuringMachine::MoveHeadRight() {
   int ret = -1;

   auto next = _headPositionIter;
   auto now = next;
   int index = std::get<1>(*now);
   std::string symbol = std::get<0>(*now);

   // if at last position (end() is last + 1)
   // check and add 
   ++next;
   if(next == _tape.end()) { 

      index++;
      TapeData td = std::make_tuple(TAPE_BLANK_SYMBOL, index);
      _tape.push_back(td);

      ++_headPositionIter;
      ret = 0;
   }
   else if(now == _tape.begin()) { // check front

      // if first is not B then add new B
      if(TAPE_BLANK_SYMBOL != symbol){

         index--;
         TapeData td = std::make_tuple(TAPE_BLANK_SYMBOL, index);
         _tape.push_front(td);

      } // end if 

      ++_headPositionIter;
      ret = 0;
   }
   else { // at middle so just move pointer 
      ++_headPositionIter;
      ret = 0;
   } // end if 

   return ret;
} // end MoveHeadRight


// MoveHeadLeft consider both two way and standard tapes, also 
// consider if the head is at the begin ot last (end()-1) tape position
// return 0 on success
// return -1 if two_way and at left boundary symbol or 
//           if standard and at left most blank
int TuringMachine::MoveHeadLeft() {
   int ret = 0;

   auto next = _headPositionIter;
   auto now = next;
   int index = std::get<1>(*now);
   std::string symbol = std::get<0>(*now);

   // if at last position (end() is last + 1)
   // check and add 
   ++next;
   if(next == _tape.end()) {

      // add B if last wasn't already B
      if(TAPE_BLANK_SYMBOL != symbol) {

         index++;
         TapeData td = std::make_tuple(TAPE_BLANK_SYMBOL, index);
         _tape.push_back(td);

      } // end if 

      --_headPositionIter;
      ret = 0;
   } 
   else if(now == _tape.begin()) { // check front

      // if at begin and two way 
      if(TmConfiguration::TwoWay == _configurationType) {

         // add a B to the beginning and move left
         // ignore left Bs
         if(TWO_WAY_LEFT_BOUNDARY != symbol) {

            index--;
            TapeData td = std::make_tuple(TAPE_BLANK_SYMBOL, index);
            _tape.push_front(td);

            --_headPositionIter;
            ret = 0;
         }
         else {
            // can't move left 
            ret = -1;
         } // end if 

      }
      else { // standard type (variant)

         // if first is not B then add new B
         if(TAPE_BLANK_SYMBOL != symbol) {

            index--;
            TapeData td = std::make_tuple(TAPE_BLANK_SYMBOL, index);
            _tape.push_front(td);

            --_headPositionIter;
            ret = 0;
         }
         else {
            // can't move left 
            ret = -1;
         } // end if 

      } // end if 

   } 
   else { // at middle so just move pointer 
      --_headPositionIter;
      ret = 0;
   } // end if 

   return ret;
} // end MoveHeadLeft

// TransitionStep(), this function does the TM actions,
// 1) read the tape symbol, 2) lookup the transition for the current 
// state and symbol, 3) write the new symbol, and 4) move the head.
// remarks: it only has success and fail as return values, but it sets 
// the _status member (type TmStatus) on each call. use GetTmStatus() 
// for the detailed status 
// return 0 success
// return -1 something went wrong 
// sets _status to:
// TmStatus::InProgress, valid next transition
// TmStatus::CompleteOnEmptyTransition, no final states
// TmStatus::AcceptedOnFinalState, if there are final states
// TmStatus::RejectedNotOnFinalState, if there are final states
// TmStatus::InvalidLeftMove, standard left on position 0 or two-way left on #
// TmStatus::SomethingWentWrong, should not happen, but ?
int TuringMachine::TransitionStep() {
   int ret = 0;

   _status = TmStatus::InProgress;
   std::string nextState;

   // check if in range, should always work so assert on fail 
   if(_headPositionIter == _tape.end()){
      assert(false);
   } // end if 

   // get the tape symbol
   std::string tapeSymbol;
   tapeSymbol = std::get<0>(*_headPositionIter);
 
   // check if a transition exists for the state (row)
   // this should always work so use an assert on fail
   // since there is structural problem with the table 
   auto rowIter = _transitionTable.find(_currentState);
   if(rowIter != _transitionTable.end()) {

      // check if there is a transtion for the tapeSymbol column 
      auto colIter = _transitionTable[_currentState].find(tapeSymbol);
      if(colIter != _transitionTable[_currentState].end()) {

         nextState = _transitionTable[_currentState][tapeSymbol].GetState();

         // update the tape symbol
         std::string newTapeSymbol = _transitionTable[_currentState][tapeSymbol].GetSymbol();
         std::get<0>(*_headPositionIter) = newTapeSymbol;

         // update the currentSymbol
         _currentSymbol = newTapeSymbol;

         // get the tape symbol and move the tape head
         int newIdx = std::get<1>(*_headPositionIter);
         std::string direction = _transitionTable[_currentState][tapeSymbol].GetDirection();
      
         // update the direction and the index
         _currentDirection = direction;
         _currentPositionIdx = newIdx;

         // all right moves are valid, 
         // in standard left of first B is invalid  
         // in two-way left of '#' is invalid, as defined by Sudkamp page 265 
         if(direction == TAPE_RIGHT_SYMBOL) {
            int result = MoveHeadRight();
         }
         else {
            int result = result = MoveHeadLeft();
            if(-1 == result) {
               _status = TmStatus::InvalidLeftMove;
            } // end if 
         } // end if 

         // update the current state 
         _currentState = nextState;
      } 
      else {
         _status = TmStatus::CompleteOnEmptyTransition;
      } // end if  // from column not found

   }
   else {
      _status = TmStatus::CompleteOnEmptyTransition;
   } // end if // from row not found


   // test if the next state is in _finalStates
   if(TmStatus::CompleteOnEmptyTransition == _status) {

      // test if in final state 
      if(_finalStates.size() > 0) {

         if(IsStateFinal(_currentState) == true) {
            _status = TmStatus::AcceptedOnFinalState;  // halt on entering final state  
         } 
         else {
            _status = TmStatus::RejectedNotOnFinalState; // halt but not on final state   
         } // end if
      } // end if 
       
   }
   // review this, it works but needs more testing  
   else if(TmStatus::InvalidLeftMove == _status &&
           TmConfiguration::Standard == _configurationType){

      // test if in final state 
      if(_finalStates.size() > 0) {

         if(IsStateFinal(_currentState) == true) {
            _status = TmStatus::AcceptedOnFinalState;  // halt on entering final state  
         } else {
            _status = TmStatus::RejectedNotOnFinalState; // halt but not on final state   
         } // end if
      } // end if 

   } // end if 

   return ret;
} // end TransitionStep


// return true state is a final state 
// return false state is not a final state 
bool TuringMachine::IsStateFinal(std::string state) {
   return _finalStates.find(state) != _finalStates.end();
} // end IsStateFinal


// return the transaction as Transaction class   
Transition TuringMachine::GetCurrentTransition() {
   Transition ret(_currentState, _currentSymbol, _currentDirection, _currentPositionIdx);
   return ret;
} // end GetTransition


// get the current state string 
std::string TuringMachine::GetState() {
   std::string ret;
   ret = _currentState;
   return ret;
} // end GetState


// get the tape head positin as an index
int TuringMachine::GetTapeHeadPosition() { 
   int ret;
   ret = _currentPositionIdx;
   return ret;
} // end GetTapeHeadPosition


// return the current Computation as a formatted string, 
// it follows Sudkamp except for parens around the state string  
std::string TuringMachine::GetComputation() {
   std::ostringstream oss;

   oss << " |- ";
   
   TapeIter iter = _tape.begin();
   for(; iter != _tape.end(); ++iter) {

      // write the current state string before the actual symbol/char   
      if(_headPositionIter == iter) {
         oss << "(" << _currentState << ")";
      } // end if 

      oss << std::get<0>(*iter);

   } // end for

   // special case on the last computation put the state last 
   if(_headPositionIter == _tape.end()) {
      oss << "(" << _currentState << ")";
   } // end if 

   return oss.str();
} // end GetComputation


// pretty print for the defintion that includes the 
// _description, _configuration, start and final states,
// transition table, and a header for the computations.
// call this before GetComputation()
StringVector TuringMachine::GetDefinition() {
   StringVector ret;
   std::ostringstream oss;

   ret.push_back("Turing Machine Simulation: " + _description);

   oss << "variant: " << _configuration;
   ret.push_back(oss.str());
   oss.str(""); // clear oss 

   oss << "start state: " << _startState;
   ret.push_back(oss.str());
   oss.str(""); // clear oss 

   // only list final states if there is final states 
   if(!_finalStates.empty()) {

      std::string finals; // string for the states 
      oss << "final state(s): " << "{";
   
      // for_each w/ lambda to build list of final states 
      for_each(_finalStates.begin(),_finalStates.end(),[&](std::string n){finals += n + ",";});
      finals = finals.substr(0,finals.size()-1); // remove the last ','
   
      oss << finals << "}";
      ret.push_back(oss.str());
      oss.str(""); // clear oss 
   } // end if 

   ret.push_back(" ");
   ret.push_back("transition table: ");

   // add the raw table from the definition file 
   std::for_each(_definition.begin(),_definition.end(), [&](std::string s){
      ret.push_back(s);
   });

   ret.push_back(" ");
   ret.push_back("computation: ");

   oss << "(" << _startState << ")" << TAPE_BLANK_SYMBOL  
       << _inputString << TAPE_BLANK_SYMBOL;
   ret.push_back(oss.str());

   return ret;
} // end GetDefinition


// GetFirstTapePosition() and GetNextTapePosition() work as a pair to get the
// TapeData on the tape link list. use these two with a while loop to get all
// positions without having using an index.  
bool TuringMachine::GetFirstTapePosition(TapeData &tapeData) {
   bool ret = false;

   // if not empty get the first tapeDate  
   // else return false  
   if(_tape.empty() == false) {
      _tapeIter = _tape.begin();
      tapeData = (*_tapeIter);
      ret = true;
   } // end if 

   return ret;
} // end GetFirst

// GetFirstTapePosition() and GetNextTapePosition() work as a pair to get the
// TapeData on the tape link list. use these two with a while loop to get all
// positions without having using an index.  
bool TuringMachine::GetNextTapePosition(TapeData &tapeData){
   bool ret = false;

   // double check that _tapeIter is not at end before incrementing 
   if(_tapeIter != _tape.end()){
      ++_tapeIter;
   }
   else { 
      return false;
   } // end if 

   // if not empty get the first tapeDate  
   // else return false  
   if(_tapeIter != _tape.end()) {
      tapeData = (*_tapeIter);
      ret = true;
   } // end if 

   return ret;
} // end GetNext


// bcook 12-08-2021
int TuringMachine::WriteGraphvizDotFile(std::string fname) {

   using namespace boost::filesystem;
   using namespace std;
   using std::begin;
   using std::end;

   int ret = 0;

   std::set<string> stateList;
   map<string, string> stateToStateTransition;

   // TmTransitionTable::iterator tableIter;
   for(auto tableIter = _transitionTable.begin(); tableIter != _transitionTable.end(); ++tableIter) {
      for(auto rowIter = tableIter->second.begin(); rowIter != tableIter->second.end(); ++rowIter) {

         string fromState = tableIter->first;
         string toState = rowIter->second.GetState();
         string readSymbol = rowIter->first;
         string writeSymbol = rowIter->second.GetSymbol();
         string direction = rowIter->second.GetDirection();

         // add both from and to states, duplicates ignored
         stateList.insert({ fromState });
         stateList.insert({ toState });

         string stateToState = fromState + " -> " + toState;
         string transition = readSymbol + "/" + writeSymbol + " " + direction;
         auto found = stateToStateTransition.find(stateToState);
         if(found != stateToStateTransition.end()) {
            found->second += "\\n" + transition;
         }
         else {
            stateToStateTransition.emplace(make_pair(stateToState, transition));
         } // end if 

      } // end for
   } // end for 


   // create a graphviz dot file folder
   path graphviz_folder{"gv_files"};
   try {
      create_directory(absolute(graphviz_folder));
   }
   catch(filesystem_error &e) {
      std::cerr << e.what() << '\n';
      return -1;
   } // end try/catch

   // make a graphviz file name
   path fullpath = current_path();
   fullpath /= graphviz_folder;
   fullpath /= string(fname + ".gv");

   // open the file and overwrite if already exists 
   std::ofstream outfile;
   outfile.open(fullpath.c_str(), std::ofstream::trunc);
   if(outfile.fail()) {
      return -2;
   } // end if 

   outfile << "digraph { \n // dot -Tsvg addition.gv -o addition.svg \n   rankdir = LR" << endl;

   for_each(begin(stateList), end(stateList), [&](string item) {outfile << "   " << item << "[color = \"#000003\"];" << endl; });
   for_each(begin(stateToStateTransition), end(stateToStateTransition), [&](auto const &mpair) {
      outfile << "   " << mpair.first << " [label=\"" << mpair.second << "\"]; "<< endl; }
   );

   outfile << "}" << endl;
   outfile.close();

   return ret;
} // end WriteGraphvizDotFile


