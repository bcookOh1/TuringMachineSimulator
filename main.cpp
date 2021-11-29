// file: main.cpp
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: entry point for the main window
//

#include "MainWind.h"

// event trap callback to ignore the escape key press 
// ref: http://www.fltk.org/articles.php?L378+I0+TFAQ+P1+Q
void IgnoreEscCB(Fl_Widget*, void*) {
   if(Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape)
      return; // ignore Escape
   exit(0);
} // end IgnoreEscCB

// the main, intantiate a MainWind, show and run it
int main(int argc, char **argv) {
   MainWind mainWind(578, 570, "Turing Machine Simulator");
   mainWind.callback(IgnoreEscCB);
   mainWind.show();
   return (Fl::run());
} // end main 

