// file: Transition.h
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: implementation for a cell in the transition table.
// There is the addition copy constructor and assignment operator
// that is needed when use in a stl container class.  

#include "Transition.h"


Transition::Transition() {
   _index = 0;
} // end ctor 


Transition::Transition(std::string state,
                       std::string symbol,
                       std::string direction,
                       int index) {
   SetStateSymbolAndDirectionForState(state, symbol, direction, index);
} // end ctor 


Transition::~Transition() {
} // end dtor


// copy ctor 
Transition::Transition(const Transition &rhs) {
   _state = rhs._state;
   _symbol = rhs._symbol;
   _stepDirection = rhs._stepDirection;
   _index = rhs._index;
} // end copy ctor 


// over assignment operator 
Transition& Transition::operator=(const Transition &rhs) {
   if(this == &rhs) return *this;
   _state = rhs._state;
   _symbol = rhs._symbol;
   _stepDirection = rhs._stepDirection;
   _index = rhs._index;
   return *this;
} // end assignment oper ator 


int Transition::SetStateSymbolAndDirectionForState(std::string state,
                                                   std::string symbol,
                                                   std::string direction,
                                                   int index) {
   int ret = 0;
   
   _state = state;
   _symbol = symbol;
   _stepDirection = direction;
   _index = index;

   return ret;
} // end SetStateSymbolAndDirectionForState


