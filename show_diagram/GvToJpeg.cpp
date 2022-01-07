

#include "GvToJpeg.h"

GvToJpeg::GvToJpeg() {}
GvToJpeg::~GvToJpeg() {}


int GvToJpeg::ConvertGv(const string &gvFile) {
   int ret = 0;
   _errorStr = "";

   _gvFile = gvFile;
   //_imgFolder = imgFolder;

   ret = ReadStatesFromGv();
   if(ret == 0) {

      ConvertDotFile cdf;
      ret = cdf.Init();
      if(ret != 0) {
         _errorStr = "ConvertDotFile Init failed";
         return -1;
      } // end if 

      string svgFile;

      ret = cdf.DotFileToSvg(gvFile, svgFile);
      if(ret != 0) {
         _errorStr = "DotFileToSvg failed with: " + boost::lexical_cast<string>(ret);
         return -1;
      } // end if 

      // use  the original svg used to make all states off jpg
      string allOffJpg;
      ret = ConvertSvgToJpg(svgFile, allOffJpg);
      if(ret != 0) {
         _errorStr = "DotFileToSvg: failed to convert all off jpg";
         return -1;
      } // end if 

      _jpgFiles.clear();

      for(string stateName : _tmStates) {
         string newSvg;
         ret = MakeSvgForState(stateName, svgFile, newSvg);
         if(ret == 0) {

            string jpgFile;
            ret = ConvertSvgToJpg(newSvg, jpgFile);
            if(ret == 0) {
               _jpgFiles.insert(make_pair(stateName, jpgFile));
            }
            else {
               break;
            } // end if 
         }
         else {
            break;
         } // end if 
      } // end for 

      // add the all states off jpg to the map
      if(ret == 0){
         _jpgFiles.insert(make_pair(AllOffTm, allOffJpg));
      } // end if 

   } // end if 

   return ret;
} // end ConvertGv


int GvToJpeg::GetJpgFromStateName(const string &stateName, string &jpgFile) {
   int ret = 0;

   auto iter = _jpgFiles.find(stateName);
   if(iter != _jpgFiles.end()) {
      jpgFile = iter->second;
   }
   else {
      ret = -1;
   } // end if 

   return ret;
} // end GetJpgFromStateName


int GvToJpeg::GetStateNameAndJpgFromIndex(size_t idx, string &stateName, string &jpgFile) {
   int ret = 0;

   if(idx < _jpgFiles.size()){
      auto iter = _jpgFiles.begin();
      advance(iter, idx);
      stateName = iter->first;
      jpgFile = iter->second;
   }
   else {
      ret = -1;
   } // end if 

   return ret;
} // end GetStateNameAndJpgFromIndex



int GvToJpeg::ReadStatesFromGv() {
   int ret = 0;

   _tmStates.clear();
   string line;

   std::ifstream in(_gvFile);
   if(in.is_open()) {

      // read the file a line at a time until StatesCommentPrefex is found 
      // at the beginning then split state strings into vector<string>. 
      // assume StatesCommentPrefex is at the beginning of a line 
      while(in.good()) {
         std::getline(in, line);

         auto res = find_first(line, StatesCommentPrefex);
         if(res) { // read lines until found, split then break
            erase_first(line, StatesCommentPrefex);
            trim_left(line);
            trim_right(line);
            split(_tmStates, line, is_any_of(","));
            break;
         } // end if 
      } // end while

      in.close();
   }
   else {
      _errorStr = "ReadStatesFromGv: cannot open the file: " + _gvFile;
      ret = -1; // can't open file 
   } // end if 

   return ret;
} // end ReadStatesFromGv


int GvToJpeg::MakeSvgForState(const string &stateName, const string &svgFile, string &newSvg) {
   int ret = 0;
   bool found = false;

   _errorStr = "";

   pugi::xml_document doc;
   pugi::xml_parse_result result = doc.load_file(svgFile.c_str());
   if(!result) {
      ret = -1;
      _errorStr = "MakeSvgForState: failed to open the file: " + svgFile;
   }
   else {

      // set the active state ellipse color by brute force, refactor if possible 
      for(pugi::xml_node state: doc.child("svg").child("g").children("g")) {
         if(state.attribute("class").as_string() == NodeString) {

            // title text contains the tm state as a string 
            string title = state.child("title").text().as_string();

            // the stroke attribute in the ellipse node, made a var since 
            // it's used in multiple places  
            auto fill = state.child("ellipse").attribute("fill");

            // set the active state ellipse fill color to tomato else black 
            if(title == stateName) {
               fill.set_value(RedFillAttribStr.c_str());
               found = true;
            }
            else {
               fill.set_value(NoFillAttribStr.c_str());
            } // end if 

         } // end if

      } // end for 

      // save the new svg
      if(found == true) {
         newSvg = svgFile;

         size_t pos = newSvg.find(SvgFileExtension.c_str());
         newSvg.insert(pos, "_" + stateName);
         if(doc.save_file(newSvg.c_str()) == false) {
            ret = -1;
            _errorStr = "MakeSvgForState: failed to write the file: " + newSvg;
         } // end if
      }
      else {
         ret = -1;
         _errorStr = "MakeSvgForState: active state not found: " + stateName;
      } // end if 

   } // end if 

   return ret;
} // end MakeSvgForState




int GvToJpeg::ConvertSvgToJpg(const std::string &svgFile, string &jpgFile) {
   int ret = 0;

   // magick mogrify -format jpg *.svg
   jpgFile = replace_last_copy(svgFile, SvgFileExtension, JpgFileExtension);
   string cmd = "rsvg-convert.exe " + svgFile + " -o " + jpgFile;
   system(cmd.c_str());
 
   return ret;
} // end ConvertSvgToJpg


