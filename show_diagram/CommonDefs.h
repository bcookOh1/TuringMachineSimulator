#pragma once

#include <string>
#include <tuple>

using namespace std;


const string NodeString = "node";
const string TitleString = "title";
const string StatesCommentPrefex = "// states:";

const string NoFillAttribStr = "none";
const string RedFillAttribStr = "tomato";

const string AllOffTm = "allOff";
const string RelativeImgFolder = "./img";

// include the '.' as part of extention, easier when 
// searching or replacing 
const string GvFileExtension = ".gv";
const string SvgFileExtension = ".svg";
const string JpgFileExtension = ".jpg";

enum class IpcqCmd : unsigned int {
   Close = 0,
   GvFile,
   NewState
}; // end enum


// window/box/image sizing 
const int Inside_Border = 10;
const int MainWind_W_Max = 1220;
const int MainWind_H_Max = 620;
const int MainWind_W_Min = 620;
const int MainWind_H_Min = 320;

const int Box_W_Max = MainWind_W_Max - (2 * Inside_Border);
const int Box_H_Max = MainWind_H_Max - (2 * Inside_Border);
const int Box_W_Min = MainWind_W_Min - (2 * Inside_Border);
const int Box_H_Min = MainWind_H_Min - (2 * Inside_Border);

inline std::tuple<int,int> BoxDimsFromWind(int w, int h){
   std::tuple<int,int> ret{std::make_tuple(w-(2 * Inside_Border),h-(2 * Inside_Border))}; 
   return ret;
} // end BoxDimsFromWind

inline std::tuple<int,int> WindDimsFromJpg(int w, int h){
   std::tuple<int,int> ret{std::make_tuple(w+(2 * Inside_Border),h+(2 * Inside_Border))}; 
   return ret;
} // end WindDimsFromJpg
