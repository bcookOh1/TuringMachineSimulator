// file: Transition.h
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: header a cell in the transition table.
// there is an addition _index member that is a convenience 
// when return info to the tape GUI    
// Description: implementation for a cell in the transition table.
// There is the addition copy constructor and assignment operator
// that is needed when use in a stl container class.  
// note: see the .cpp implementation for function comments


#pragma once

#include <string>
#include <iostream>
#include <sstream>


class Transition {
private:

   std::string _state;
   std::string _symbol;
   std::string _stepDirection;
   int _index;

public:
   Transition();
   Transition(std::string state, std::string symbol, 
              std::string direction, int index = 0);
   ~Transition();

   // copy ctor 
   Transition(const Transition &rhs);

   // over assignment operator 
   Transition& operator=(const Transition &rhs);

   int SetStateSymbolAndDirectionForState(std::string state, 
                                          std::string symbol, 
                                          std::string direction,
                                          int index = 0);

   std::string GetState() {return _state;}
   std::string GetSymbol() {return _symbol;}
   std::string GetDirection() {return _stepDirection;}
   int GetIndex() { return _index; }

   // bcook 12-7-2021 debug function 
   std::string toStr() {
      std::ostringstream oss;
      oss << "state: "<< _state << ", symbol: " << _symbol << ", step dir: " << _stepDirection << ", index: " << _index;
      return oss.str();
   } // end toStr

}; // end class 


