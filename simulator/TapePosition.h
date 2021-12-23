// file: TapePosition.h
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: header definition for FLTK box used to simulate
// a tape position in the Turing Machine visual simulator 
// note: see the .cpp implementation for function comments


#pragma once

#include "FL\Fl_Group.H"
#include <FL\Fl_Box.H>
#include <FL\Fl_Output.H>


class TapePosition : public Fl_Group {
private:

   Fl_Box* _boxBorder;
   Fl_Output* _outValue;
   Fl_Output* _outIndex;

   char _value;
   int _index;

public:

   TapePosition(int x, int y, int w, int h, const char* l = 0);
   ~TapePosition();

   char GetValue() { return _value; }
   void SetValue(char value);

   int GetIndex() { return _index; }
   void SetIndex(int index);

}; // end class


