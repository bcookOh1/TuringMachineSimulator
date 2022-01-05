// file: main.cpp
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: entry point for the main window
//

#include <iostream>
#include "MainWind.h"


// the main, intantiate a MainWind, show and run it
int main(int argc, char **argv) {
   MainWind mainWind(578, 620, "Turing Machine Simulator"); // add 50 to height // old 570
   mainWind.show();
   return (Fl::run());
} // end main 

