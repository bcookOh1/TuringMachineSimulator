

#include "MainWind.h"


MainWind::MainWind(int w, int h, const char* title, string gvFile) :
 Fl_Double_Window(20, 20, w, h, title) {

    _idx = 0;
    _gvFile = gvFile;

   _box = new Fl_Box(10,10,w-10,h-10);
   _jpg = new Fl_JPEG_Image("loading.jpg");  

   end(); // fltk call, ends additions to gui tree

   _box->image(_jpg);
   _jpg->scale(_box->w(), _box->h());

   _ipcqReader = new ipcq::IpcQueueReader(QueueName);
   _ipcqIsOpen = false;

   Fl::add_timeout(1.0, StartupTimerCB, this);

} // end ctor 


MainWind::~MainWind(){
   delete _ipcqReader;
} // end dtor


int MainWind::handle(int msg){
   int ret = 0;

   if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Down)  {
      LoopImages();
      ret = 1;
   } // end if 

   return ret;
} // end handle

void MainWind::StartupTimerCB(void *param) {

   Fl::remove_timeout(StartupTimerCB, param);

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(param);

   auto start = high_resolution_clock::now();

   mainwind->LoadImages();
   mainwind->ShowImageForState(AllOffTm);

   auto stop = high_resolution_clock::now();
   auto duration = duration_cast<milliseconds>(stop - start);
   cout << "load images duration: " << duration.count() << " ms" << std::endl;

   // open the ipcq for reading  
   int result = mainwind->_ipcqReader->Open();
   if(result == 0){
     mainwind->_ipcqIsOpen = true;
   }
   else {
      mainwind->_ipcqIsOpen = false;
      cout << mainwind->_ipcqReader->GetErrorStr() << endl;
   } // end if 

   Fl::add_timeout(1.0, ReadActiveStateCB, param);

   return;
} // StartupTimerCB


void MainWind::ReadActiveStateCB(void *param) {

   Fl::remove_timeout(StartupTimerCB, param);

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(param);
   mainwind->ReadActiveStateFromIpcq();

   Fl::add_timeout(.09, ReadActiveStateCB, param);

   return;
} // end ReadActiveStateCB


int MainWind::ReadActiveStateFromIpcq(){
   int ret = -1;

   if(_ipcqIsOpen == true){
      unsigned int cmd = 0;
      string state;

      int result = _ipcqReader->Pop(cmd, state);
      if(result == 0){
         if(cmd == 1){
            result = ShowImageForState(state);
         } // end if 
      }
      else {
         cout << _ipcqReader->GetErrorStr() << endl;
      } // end if 
      
   } // end if 

   return ret;
} // end ReadActiveStateFromIpcq

int MainWind::LoadImages(){
   int ret = 0;

   int result = _gvtojpg.ConvertGv(_gvFile);
   if(result != 0) {
      ret = -1;
      cout << _gvtojpg.GetErrorString() << endl;
   }
   else {

      string n, f;
      for(size_t i = 0; i < _gvtojpg.GetImageCount(); i++) {
         _gvtojpg.GetStateNameAndJpgFromIndex(i, n, f);
         _images.insert(std::make_pair(n, new Fl_JPEG_Image(f.c_str())));
      } // end if 

   } // end if 

   DeleteTemporaryImages();

   return ret;
} // end initialize


int MainWind::ShowImageForState(const string &state){
   int ret = 0;

   auto iter = _images.find(state);
   if(iter != _images.end()){
      _jpg = iter->second;
      _box->image(_jpg);
      _jpg->scale(_box->w(), _box->h());
      _box->redraw();
   }
   else {
      ret = -1;
   } // end if 

   return ret;
} // end ShowImageForState


int MainWind::LoopImages(){
   int ret = 0;

   _idx = (_idx >= _gvtojpg.GetImageCount() ? 0 : _idx);
   string stateName;
   string jpgFile;
   _gvtojpg.GetStateNameAndJpgFromIndex(_idx, stateName, jpgFile);
   ShowImageForState(stateName);
   _idx++;

   return ret;
} // end LoopImages


int MainWind::DeleteTemporaryImages(){
   int ret = 0;

   string key, jpgfile, svgFile;
   for(size_t i = 0; i < _gvtojpg.GetImageCount(); i++) {

      _gvtojpg.GetStateNameAndJpgFromIndex(i, key, jpgfile);
      svgFile = replace_last_copy(jpgfile, JpgFileExtension, SvgFileExtension);

      std::remove(jpgfile.c_str());
      std::remove(svgFile.c_str());

   } // end if 

   return ret;
} // end DeleteTemporaryImages



