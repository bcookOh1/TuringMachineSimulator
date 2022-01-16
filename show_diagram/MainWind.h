// file: MainWind.h

#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_PNG_Image.H>
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <future>
#include <atomic>
#include <ipcq.h>
#include "CommonDefs.h"
#include "GvToImage.h"


using namespace std;
using namespace boost::algorithm;
using namespace std::chrono;
namespace fs = std::filesystem;


class MainWind : public Fl_Double_Window {
public:

   MainWind(int w, int h, const char* title);
   ~MainWind();

   // event handler for down arrow to roll images,
   // called by the event framework
   int handle(int msg);

private:

   size_t _imageLoopIdx;
   int LoopImages();

   Fl_Box *_box; 
   Fl_PNG_Image *_img; 
   Fl_PNG_Image *_loading; 

   GvToImage _gvtoimg;

   string _title;
   string _gvFile;
   map<string, unique_ptr<Fl_PNG_Image>> _images;

   ipcq::IpcQueueReader *_ipcqReader;
   bool _ipcqIsOpen;
   future<int> _futImagesLoaded;

   int ReadIpcq();

   // callbacks 
   static void StartupTimerCB(void *param);
   static void ReadIpcqCB(void *param);
   static void LoadImageTimerCB(void *param);
   static void MainWindOnCloseCB(Fl_Widget *wind, void *data);

   int ShowLoadingImage();
   int LoadImagesAsync();
   int ConvertGvAndLoadImages();
   int ShowImageForState(const string &state = AllOffTm);
   int DeleteTemporaryImages();

   int ResizeMainWindAndBox();

}; // end class

