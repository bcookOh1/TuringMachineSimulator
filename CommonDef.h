// file: CommonDef.h
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: header for common definition used in the Turing Machine Simulator 
// Dependencies: D:\sw_libs\c++\boost_1_63_0, D:\sw_libs\c++\fltk-1.3.4-2
//

#pragma once

#include <string>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <boost/xpressive/xpressive.hpp>

using namespace boost;
using namespace boost::xpressive;

// forwards 
class Transition;

// constants 
const std::string CONFIGURATION_STANDARD = "standard";
const std::string CONFIGURATION_TWO_WAY = "two_way";
const std::string TRANSITION_FUNCTION_TABLE_BEGIN_LABEL = "transition_function_table_begin:";
const std::string TRANSITION_FUNCTION_TABLE_END_LABEL = "transition_function_table_end:";
const std::string COMPUTATION_FILE_NAME = "computation_%.txt";
const std::string PLACEHOLDER = "%";
const std::string CELL_DELIMITER = "|";
const std::string EMPTY_LABEL = "-";
const std::string DELTA = "delta";
const std::string INPUT_STRING_LABEL = "input string";
const std::size_t TABLE_CELL_ITEM_COUNT = 3;
const std::string TAPE_BLANK_SYMBOL = "B";
const std::string TWO_WAY_LEFT_BOUNDARY = "#";
const std::string TAPE_LEFT_SYMBOL = "L";
const std::string TAPE_RIGHT_SYMBOL = "R";

// boost xpressive sregex constants for reading the definition file 
const sregex Sre_Configuration = as_xpr("variant:") >> 
                                 xpressive::optional(" ") >> 
                                 (s1= as_xpr(CONFIGURATION_STANDARD) | as_xpr(CONFIGURATION_TWO_WAY));

const sregex Sre_Description = as_xpr("description:") >> 
                               xpressive::optional(" ") >> 
                               (s1= +_);

const sregex Sre_Input_Example = as_xpr("input_example:") >> 
                                 xpressive::optional(" ") >> 
                                 (s1= +_);

const sregex Sre_Tape_Symbols = as_xpr("tape_symbols:") >>
                                xpressive::optional(" ") >>
                                as_xpr('{') >> (s1 = +_) >> as_xpr('}');

const sregex Sre_Alphabet = as_xpr("alphabet:") >>
                            xpressive::optional(" ") >>
                             as_xpr('{') >> (s1 = +set[alnum|',']) >> as_xpr('}');

const sregex Sre_Start_State = as_xpr("start:") >>
                               xpressive::optional(" ") >>
                               (s1 = +alnum);

const sregex Sre_Two_Way_Boundary = as_xpr("two_way_boundary:") >>
                                    xpressive::optional(" ") >>
                                    (s1 = _);

const sregex Sre_Final_States = as_xpr("final:") >>
                                xpressive::optional(" ") >>
                                as_xpr('{') >> (s1 = +set[alnum | ',']) >> as_xpr('}');


// this just says look for delta and return s1 as the remainder of the line 
const sregex Sre_Table_Header = as_xpr("delta") >> (s1= +_);   

// typedefs 
typedef std::vector<std::string> StringVector;
typedef std::set<std::string> StringSet;

// table row:          state            alphabet+    state,symbol,direction
typedef std::pair<std::string,std::map<std::string, Transition>> TransitionRow;

// http://en.cppreference.com/w/cpp/utility/pair
// the table         state                  the map of transitions for each alpha/symbol column      
typedef std::map<TransitionRow::first_type, TransitionRow::second_type> TmTransitionTable;

// TapeData           symbol    index 
typedef std::tuple<std::string, int> TapeData;
typedef std::list<TapeData> TapeList;
typedef std::list<TapeData>::iterator TapeIter;

//                   state        symbol       direction
typedef std::tuple<std::string, std::string, std::string> TransitionData;

// used for the gui rate dropbox 
typedef std::unordered_map<std::string, float> RateMap;
typedef std::pair<std::string, float> RatePair;

// a map containing the step rate label and times in ms
// ref: http://en.cppreference.com/w/cpp/language/list_initialization
const RateMap Rates = {{"slow",1.0f},{"medium",0.5f},{"fast",0.25f},{"turbo",0.1f}};

// enum for TuringMachine class
enum class TmStatus : int {
   Uninitialized,
   Initialized,
   InProgress,
   UserCanceled,
   CompleteOnEmptyTransition,
   AcceptedOnFinalState,
   RejectedNotOnFinalState,
   InvalidLeftMove,
   SomethingWentWrong
}; // end enum

// enum for TuringMachine class
enum class TmConfiguration : int {
   Standard,
   TwoWay
}; // end enum


// enum for GUI run State box 
enum class RunState : int {
   Off,
   CompleteOnEmptyTransition,
   AcceptedOnFinalState,
   RejectedNotOnFinalState,
   Canceled,
   Running,
   InvalidLeftMove,
   SomethingWentWrong
}; // end enum

// text for the Run State Label this matches the RunState enum
const StringVector RunStateText = {"Off" ,
                                   "HaltOnNoTransition",
                                   "AcceptedOnFinalState",
                                   "RejectedNotOnFinalState",
                                   "UserCanceled",
                                   "Running",
                                   "InvalidLeftMove",
                                   "SomethingWentWrong"};


// example file definition file 
/*
variant: standard|two_way
description: some text, 50 chars max
start: q1
tape_symbols: {B}
alphabet: {a,b}
input_example: abbabba
transition_function_table_begin:
delta	 B	      a	       b
q0	   q1,B,R	empty	empty
q1	   q2,B,L	q1,b,R	q1,a,R
q2	   q2,a,L	q2,b,L   empty
transition_function_table_end:

variant: standard
description: binary_addition
start: q0
final: {q2}
tape_symbols: {B,#}
alphabet: {a,b}
input_example: abbabba
transition_function_table_begin:
delta |   B    |   a    |  b
q0    | q1,B,R |   -    |  -
q1    | q2,B,L | q1,b,R | q1,a,R
q2    |   -    | q2,a,L | q2,b,L
transition_function_table_end:
*/


