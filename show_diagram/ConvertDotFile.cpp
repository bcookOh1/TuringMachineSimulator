
#include "ConvertDotFile.h"

ConvertDotFile::ConvertDotFile(){
   _gvc = nullptr;
} // end ctor 

ConvertDotFile::~ConvertDotFile(){
   if(_gvc == nullptr) return;
   gvFreeContext(_gvc);
} // end dtor 

int ConvertDotFile::Init(){
   int ret = 0;

   // do once for class duration
   _gvc = gvContext();
   if(_gvc == nullptr){
      ret = -1;
      _errorStr = "failed to open gv context";
   } // end if

   return ret;
} // Init

int ConvertDotFile::DotFileToSvg(const string &gvFile, string &svgFile) {
   int ret = 0;

   Agraph_t *g = nullptr;
   FILE *in = nullptr;

   // open dot file
   in = fopen(gvFile.c_str(), "r");
   if(in != nullptr){

      // read file as a dot graph
      g = agread(in, 0);
      fclose(in); // close in file here even in case of error
      if(g != nullptr){

         // layout from dot, non-zero return is an error
         int result = gvLayout(_gvc, g, "dot");
         if(result == 0) {

            svgFile = replace_last_copy(gvFile, GvFileExtension, SvgFileExtension);

            result = gvRenderFilename(_gvc, g, "svg", svgFile.c_str());
            if(result != 0) {
               ret = -1; // render failed
               _errorStr = "DotFileToSvg: render failed";
            } // end if

            gvFreeLayout(_gvc, g);
            agclose(g);

         }
         else{
            ret = -1; // layout failed
            _errorStr = "DotFileToSvg: layout failed";
         } // end if
      }
      else {
         ret = -1; // dot file readin failed
      _errorStr = "DotFileToSvg: failed to open gv file";
      } // end if
   }
   else {
      ret = -1; // fopen failed
      _errorStr = "DotFileToSvg: failed to open gv file";
   } // end if

   return ret;
} // end DotFileToSvg


