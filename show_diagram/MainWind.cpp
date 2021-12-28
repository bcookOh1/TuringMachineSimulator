

#include "MainWind.h"


MainWind::MainWind(int w, int h, const char* title) :
 Fl_Double_Window(20, 20, w, h, title) {

   _idx = 0;

   this->callback(MainWindOnCloseCB);

   _box = new Fl_Box(10,10,w-10,h-10);

   end(); // fltk call, ends additions to gui tree

   ShowLoadJpg();

   _ipcqReader = new ipcq::IpcQueueReader(QueueName);
   _ipcqIsOpen = false;

   Fl::add_timeout(1.0, StartupTimerCB, this);

} // end ctor 


MainWind::~MainWind(){
   delete _ipcqReader;
} // end dtor


// event trap callback to ignore the escape key press 
// ref: http://www.fltk.org/articles.php?L378+I0+TFAQ+P1+Q
void MainWind::MainWindOnCloseCB(Fl_Widget *wind, void *data) {
   if(Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape)
      return; // ignore Escape

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(wind);
   mainwind->hide();

} // end MainWindOnCloseCB


// handles keyboard down arrow to loop through images
int MainWind::handle(int msg){
   int ret = 0;

   if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Down)  {
      LoopImages();
      ret = 1;
   } // end if 

   return ret;
} // end handle


int MainWind::ShowLoadJpg(){
   int ret = 0;

   _jpg = new Fl_JPEG_Image("loading.jpg");  
   _box->image(_jpg);
   _jpg->scale(_box->w(), _box->h());

   return ret;
} // end ShowLoadJpg


void MainWind::StartupTimerCB(void *param) {

   Fl::remove_timeout(StartupTimerCB, param);

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(param);

   // open the ipcq for reading  
   int result = mainwind->_ipcqReader->Open();
   if(result == 0){
     mainwind->_ipcqIsOpen = true;
   }
   else {
      mainwind->_ipcqIsOpen = false;
      cout << mainwind->_ipcqReader->GetErrorStr() << endl;
   } // end if 

   // this_thread::sleep_for(chrono::milliseconds(30000));

   Fl::add_timeout(1.0, ReadIpcqCB, param);

   return;
} // StartupTimerCB


void MainWind::LoadImageTimerCB(void *param){

   // cast param to MainWind* and use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(param);
   if(mainwind->_futJpgsLoaded.wait_for(0ms) == future_status::ready){
      mainwind->ShowImageForState();
      Fl::remove_timeout(LoadImageTimerCB, param);
   }
   else {
      Fl::repeat_timeout(1.0, LoadImageTimerCB, param);
   } // end if 

   return;
} // end LoadImageTimerCB


void MainWind::ReadIpcqCB(void *param) {

      // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(param);
   mainwind->ReadIpcq();

   Fl::repeat_timeout(.09, ReadIpcqCB, param);

   return;
} // end ReadIpcqCB


int MainWind::ReadIpcq(){
   int ret = -1;

   if(_ipcqIsOpen == true){
      unsigned int cmd = 0;
      string state;

      int result = _ipcqReader->Pop(cmd, state);
      if(result == 0){
         switch(cmd){
         case static_cast<unsigned int>(IpcqCmd::Close):
            hide();
            break;
         case static_cast<unsigned int>(IpcqCmd::GvFile):
            _gvFile = state;
            ShowLoadJpg();
            LoadImagesAsync();
            Fl::add_timeout(1.0, LoadImageTimerCB, this);
            break;
         case static_cast<unsigned int>(IpcqCmd::NewState):
            result = ShowImageForState(state);
            break;
         } // end switch
      } // end if 
   } // end if 

   return ret;
} // end ReadActiveStateFromIpcq


int MainWind::LoadImagesAsync(){
   int ret = 0;
   _futJpgsLoaded = std::async(launch::async, [&]() -> int { return ConvertGvAndLoadJpgs(); });
   return ret;
} // end LoadImagesAsync


// ret -1 on error 
// ret >= 0 on number of images loaded  
int MainWind::ConvertGvAndLoadJpgs(){
   int ret = 0;

   auto start = high_resolution_clock::now(); 
   _images.clear();

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

      ret = static_cast<int>(_images.size());

   } // end if 

   DeleteTemporaryImages();

   auto stop = high_resolution_clock::now();
   auto duration = duration_cast<milliseconds>(stop - start);
   cout << "load images duration: " << duration.count() << " ms" << std::endl;

   return ret;
} // end ConvertGvAndLoadJpgs


int MainWind::ShowImageForState(const string &state){
   int ret = 0;

   auto iter = _images.find(state);
   if(iter != _images.end()){
      _jpg = iter->second;
      AppAndImageResize();
   }
   else {
      ret = -1;
   } // end if 

   return ret;
} // end ShowImageForState


int MainWind::AppAndImageResize(){
   int ret = 0;

   int img_w = _jpg->w();
   int img_h = _jpg->h(); 

   if(img_w > Box_W_Max || img_h > Box_H_Max ) { // scale jpg to max
      this->size(MainWind_W_Max, MainWind_H_Max);
   }
   else if (img_w < Box_W_Min || img_h < Box_H_Min ) { // scale jpg to min
      this->size(MainWind_W_Min, MainWind_H_Min);
   }
   else {
      auto w_h = WindDimsFromJpg(img_w, img_h);
      this->size(std::get<0>(w_h), std::get<1>(w_h));
   } // end if 

   auto w_h = BoxDimsFromWind(this->w(), this->h());
   _box->resize(Inside_Border, Inside_Border, std::get<0>(w_h), std::get<1>(w_h));
   _jpg->scale(_box->w(), _box->h(), 0);
   _box->image(_jpg);
   redraw();

   return ret;
} // end AppAndImageResize


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



