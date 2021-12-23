
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Shared_Image.H>
#include "MainWind.h"


using namespace std;

int main(int argc, char *argv[]) {

   string gvFile = "./img/BinaryAddition.gv";
   if(argc > 1){
      gvFile = argv[1]; 
   } // end if 

   fl_register_images();
   MainWind win(1126, 586, "Tm graph", gvFile);
   win.show(); 
   return(Fl::run());
} // end main

