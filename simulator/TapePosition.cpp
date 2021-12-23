// file: TapePosition.cpp
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: implementation fr FLTK Fl_Box used to simulate
// a tape position in the Turing Machine visual simulator 
//

#include "TapePosition.h"
#include <string>
#include <sstream>
#include <iomanip>


TapePosition::TapePosition(int x, int y, int w, int h, const char* l) :
   Fl_Group(x, y, w, h) {

   _boxBorder = new Fl_Box(x, y, w, h);
   _boxBorder->box(FL_BORDER_BOX);
   _boxBorder->color(fl_lighter((Fl_Color)197)); 

   _outValue = new Fl_Output(x + 8, y + 2, 25, 25);
   _outValue->box(FL_NO_BOX);
   _outValue->textfont(FL_HELVETICA);
   _outValue->textsize(24);

   _outIndex = new Fl_Output(x + 10, y + 18, 30, 20);
   _outIndex->box(FL_NO_BOX);
   _outIndex->textfont(FL_HELVETICA);
   _outIndex->textsize(10);

   end();

   return;
} // end ctor


TapePosition::~TapePosition() {
   // fltk deletes the widgets 
} // end dtor 


void TapePosition::SetValue(char value) {
   _value = value;

   // convert to null terminated const char * for the Fl_Output 
   char v[2];
   v[0] = _value;
   v[1] = 0;
   _outValue->value(const_cast<const char*>(v));

   return;
} // end SetValue


void TapePosition::SetIndex(int index) {
   _index = index;

   // use ostringstream for good formatting  
   std::ostringstream oss;

   oss << std::setw(5) << std::setfill(' ') << _index;
   _outIndex->value(oss.str().c_str());

   return;
} // end SetIndex


