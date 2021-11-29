// file: TapeHead.cpp
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: implemenation for FLTK box used to simulate
// the head for the Turing Machine visual simulator 


#include "TapeHead.h"
#include <string>
#include <sstream>
#include "boost\lexical_cast.hpp"


TapeHead::TapeHead(int x, int y)  { 

   _boxBorder = new Fl_Box(x, y, WIDTH, HEIGHT, LABEL.c_str());
   _boxBorder->box(FL_EMBOSSED_BOX);
   _boxBorder->color((Fl_Color)93);
   _boxBorder->align(Fl_Align(FL_ALIGN_INSIDE));
   _boxBorder->labelfont(FL_BOLD + FL_ITALIC);
   _boxBorder->labelsize(16);

   return;
} // end ctor 

TapeHead::~TapeHead() {
   // fltk deletes the widgets
} // end dtor 


int TapeHead::MoveTo(int x, int y) {
   int ret = 0;

   _boxBorder->hide();
   _boxBorder->position(x, y);
   _boxBorder->show();

   return ret;
} // end MoveTo

// set the head state string  
int TapeHead::SetState(std::string state) {
   int ret = 0;

   // if size > 3 truncate to first 3 chars
   if(state.size() > 3) {
      state = state.substr(0,3);
   } // end if  
       
   _boxBorder->copy_label(state.c_str());

   return ret;
} // end SetState


