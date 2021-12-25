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
   MainWind mainWind(578, 570, "Turing Machine Simulator");
   mainWind.show();
   return (Fl::run());
} // end main 

