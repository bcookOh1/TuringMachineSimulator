
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Shared_Image.H>
#include "MainWind.h"
#include "CommonDefs.h"


using namespace std;

int main(int argc, char *argv[]) {

   fl_register_images();
   MainWind win(MainWind_W_Max, MainWind_H_Max, "Tm graph");
   win.show(); 
   return(Fl::run());
} // end main

