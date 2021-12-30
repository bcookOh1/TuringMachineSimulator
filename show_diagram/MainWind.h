// file: MainWind.h

#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_JPEG_Image.H>
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
#include "GvToJpeg.h"


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
   Fl_JPEG_Image *_jpg; 
   Fl_JPEG_Image *_loading; 

   GvToJpeg _gvtojpg;

   string _title;
   string _gvFile;
   map<string, unique_ptr<Fl_JPEG_Image>> _images;

   ipcq::IpcQueueReader *_ipcqReader;
   bool _ipcqIsOpen;
   future<int> _futJpgsLoaded;

   int ReadIpcq();

   // callbacks 
   static void StartupTimerCB(void *param);
   static void ReadIpcqCB(void *param);
   static void LoadImageTimerCB(void *param);
   static void MainWindOnCloseCB(Fl_Widget *wind, void *data);

   int ShowLoadingJpg();
   int LoadImagesAsync();
   int ConvertGvAndLoadJpgs();
   int ShowImageForState(const string &state = AllOffTm);
   int DeleteTemporaryImages();

   int ResizeMainWindAndBox();

}; // end class

/*

using namespace boost::interprocess;


const string SharedMemoryName = "CPP";
const string SharedMemoryItem = "PrintEnable";

class SmallIpc : private boost::noncopyable {
public:

   SmallIpc();
   ~SmallIpc();

   int Writer(int val);

private:
}; // end SmallIpc

SmallIpc::SmallIpc() {
   shared_memory_object::remove( SharedMemoryName.c_str() );
   managed_shared_memory object{ open_or_create, SharedMemoryName.c_str(), 1024 };
   object.construct<int>(SharedMemoryItem.c_str())(1);
} // end ctor 


SmallIpc::~SmallIpc() {
   shared_memory_object::remove( SharedMemoryName.c_str() );
} // end dtor


int SmallIpc::Writer(int val) {
   managed_shared_memory object{ open_only, SharedMemoryName.c_str() };
   pair<int *, size_t> p = object.find<int>(SharedMemoryItem.c_str());

   if(p.first) {
      *p.first = val;
   } // end if 

   return 0;
} // Writer 


int IpcReader() {
   int ret = 0;

   managed_shared_memory object{ open_only, SharedMemoryName.c_str() };
   std::pair<int *, std::size_t> p = object.find<int>(SharedMemoryItem.c_str());

   if(p.first) {
      ret = *p.first;
   } // end if 

   return ret;
} // end IpcReader

*/