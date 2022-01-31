// file: MainWind.cpp
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: 
//
//

#include "MainWind.h"
#include "util.h"
#include "TuringMachine.h"
#include "Transition.h"
#include "TuringMachineFactory.h"
#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>
#include <iomanip>


// constructor 
MainWind::MainWind(int w, int h, const char* title) :
   Fl_Double_Window(20, 20, w, h, title) {
   SetupControls(w, h);
   
   this->callback(MainWindOnCloseCB);

   _tm = nullptr;
   _appStat.tm = TmStatus::Uninitialized;
   _appStat.validTm = false;
   _appStat.validInput = false;
   _appStat.complete = false;

   // declare and open the ipcq for writing
   _ipcqWriter = std::unique_ptr<ipcq::IpcQueueWriter>(new ipcq::IpcQueueWriter(QueueName));
   int result = _ipcqWriter->Open();
   if(result != 0){
      std::cout << _ipcqWriter->GetErrorStr() << std::endl;
      exit(0);
   } // end if 

   // ball backs for the state machine 
   _cfsm.SetStateMachineCB(std::bind(&MainWind::StateMachineCB, this, std::placeholders::_1));
   _cfsm.SetHasHistoryCB(std::bind(&MainWind::GetHistoryCB, this));

   _sm = std::unique_ptr<sml::sm<Cfsm>>(new sml::sm<Cfsm>{_cfsm});
   _sm->process_event(eInit{});

} // end ctor

// destructor to cleanup on exit 
MainWind::~MainWind() {
   
   if(_tm != nullptr) {
      delete _tm;
   } // end if 
   
   // fltk deletes the widgets

} // end dtor

// event trap callback to ignore the escape key press 
// ref: http://www.fltk.org/articles.php?L378+I0+TFAQ+P1+Q
void MainWind::MainWindOnCloseCB(Fl_Widget *wind, void *data) {
   if(Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape)
      return; // ignore Escape

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(wind);
   
   // 12-25-2021 close the show_diagram process
   mainwind->_ipcqWriter->Push(static_cast<unsigned int>(IpcqCmd::Close),string("none"));

   mainwind->hide();

} // end MainWindOnCloseCB


// callback from the state machine to set the control active/deactive states 
void MainWind::StateMachineCB(const BtnEnables &be){

   std::cout << "T:" << t_f(be.SelectTm) << " I:" << t_f(be.SetInputStr) << " Re:" << t_f(be.Rewind) << " B:" << t_f(be.StepBack) 
             << " P:" << t_f(be.Pause) << " F:" << t_f(be.StepForward) << " R:" << t_f(be.Run) << " S:" << t_f(be.SaveComputation) << "\n";

   SetBtnState<Fl_Button*>(_btnFileDialog, be.SelectTm);
   SetBtnState<Fl_Button*>(_btnLoadString, be.SetInputStr);
   _grpRunControls->SetRewindBtnState(be.Rewind);
   _grpRunControls->SetStepBackBtnState(be.StepBack);
   _grpRunControls->SetPauseBtnState(be.Pause);
   _grpRunControls->SetStepForwardBtnState(be.StepForward);
   _grpRunControls->SetRunBtnState(be.Run);
   SetBtnState<Fl_Button*>(_btnSaveComputation, be.SaveComputation);

} // end StateMachineCB


// callback from the state machine to return true/false 
// if_tm history is available 
bool MainWind::GetHistoryCB(){
   int size = 0;

   if(_tm != nullptr)
      size = _tm->GetHistorySize();
   
   std::cout << "history callback hit: " << size << "\n"; 
   
   return (size > 0);
} // end GetHistoryCB 


// a fuction to do the intial setup on the GUI elements 
void MainWind::SetupControls(int w, int h) {

   _outFilename = new Fl_Output(25, 25, 490, 25, "input file ");
   _outFilename->align(Fl_Align(FL_ALIGN_TOP_LEFT));
   _outFilename->value("none selected");

   _btnFileDialog = new Fl_Button(525, 20, 35, 35, "@fileopen");
   _btnFileDialog->labelsize(20);
   _btnFileDialog->callback(ShowDefinitionFileDialogCB, this);

   _lbSeq1 = new Fl_Box(560, 20,  15,  15, "1");
   _lbSeq1->labelsize(14);
   _lbSeq1->labelcolor(FL_RED);
 
   _outDescription = new Fl_Output(25, 75, 250, 25, "description ");
   _outDescription->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   _inString = new Fl_Input(290, 75, 225, 25, INPUT_STRING_LABEL.c_str());
   _inString->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   _btnLoadString = new Fl_Button(525, 75, 35, 25, "set");
   _btnLoadString->callback(UserInputStringCB, this);
   _btnLoadString->shortcut(FL_CTRL | 'b');

   _lbSeq2 = new Fl_Box(560, 71,  15,  15, "2");
   _lbSeq2->labelsize(14);
   _lbSeq2->labelcolor(FL_RED);
 
   _lbExample = new Fl_Box(363, 59, 180, 15, "ex: ");
   _lbExample->align(Fl_Align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT));
   _lbExample->hide();

   _lbRunState = new Fl_Box(331, 127, 228, 40, "ACCEPTED");
   _lbRunState->box(FL_THIN_UP_BOX);
   _lbRunState->labelfont(FL_BOLD + FL_ITALIC);
   _lbRunState->labelsize(18);
   SetRunStatusBox(TmStatus::Uninitialized);
 
   _grpRunControls = new GroupRunControls(25,120);
   _grpRunControls->callback(GroupControlCB,this);

   _lbSeq3 = new Fl_Box(315, 115, 15,  15, "3");
   _lbSeq3->labelsize(14);
   _lbSeq3->labelcolor(FL_RED);

   _grpTuringTape = new GroupTuringTape(25, 183);  

   _bwrComputation = new Fl_Browser(25, 340, 490, 220, "definition and computations");  
   _bwrComputation->textfont(4);
   _bwrComputation->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   _btnSaveComputation = new Fl_Button(525, 340, 35, 35, "@filesave");  
   _btnSaveComputation->labelsize(20);
   _btnSaveComputation->callback(ShowSaveComputationFileDialogCB, this);

   _lbSeq4 = new Fl_Box(560, 340, 15, 15, "4");  
   _lbSeq4->labelsize(14);
   _lbSeq4->labelcolor(FL_RED);

   _outStatus = new Fl_Output(5, 585, 560, 25, "status"); 
   _outStatus->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   end(); // fltk call, ends additions to gui tree

   return;
} // end SetupControls

void MainWind::GroupControlCB(Fl_Widget *widget, void *param){

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(param);

   // call the control button functions
   GroupButton clicked = mainwind->_grpRunControls->GetClickedBtn();
   switch(clicked){
   case GroupButton::Rewind:
      mainwind->InitializeTm();
      break;
   case GroupButton::Run:
      mainwind->RunButton();
      break;
   case GroupButton::Pause:
      mainwind->PauseButton();
      break;
   case GroupButton::StepForward:
      mainwind->StepForward();
      break;
   case GroupButton::StepBack:
      mainwind->StepBackward();
      break;
   default:
      break;
   }; // end switch

} // end GroupControlCB

// ShowDefinitionFileDialogCB callback for the _btnFileDialog to use the
// defintion file name to show a fikle dialog to let the user sselect a *.tm 
// file and then call the TmFactory() to create a _tm object 
void MainWind::ShowDefinitionFileDialogCB(Fl_Widget *widget, void *param) {

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(param);

   std::string desktop = GetDesktopPath();
   mainwind->SetRunStatusBox(TmStatus::Uninitialized);

   Fl_File_Chooser chooser(desktop.c_str(), "*.tm", 
                           Fl_File_Chooser::SINGLE, "Turing Machine");
   chooser.directory("./tms" /*desktop.c_str()*/);
   chooser.show();

   // block until user selects a button
   while(chooser.shown()) Fl::wait();

   // if user selected a file clear the internal status and 
   // use factory to make a new tm 
   if(chooser.value() != NULL) {

      mainwind->_appStat.validTm = false;

      // clear the input string 
      mainwind->_appStat.validInput = false;
      mainwind->_inString->value("");
      mainwind->_inString->color(FL_BACKGROUND2_COLOR);

      mainwind->SetDefinitionfile(chooser.value(0));
      mainwind->TmFactory();
      mainwind->_grpTuringTape->SetConfiguration(mainwind->_tm->GetConfigurationType());

      // use _partialfname for writing the computation file  
      std::string fname(chooser.value(0));
      GetFilenameFromPath(fname);
      mainwind->_partialfname = fname;

      WriteGraphvizDotFile(mainwind->_tm, fname, mainwind->_gvFullPath);

      // spawn a new gviz.exe process (boost::process child) 
      // if child is closed, open it
      if(mainwind->_child == nullptr) {
         mainwind->_child = unique_ptr<bp::child>(new bp::child("gviz.exe"));
      }
      else {
         // if child was closed, reopen
         if(mainwind->_child->running() == false){
            mainwind->_child = unique_ptr<bp::child>(new bp::child("gviz.exe"));
         } // end if 
      } // end if 
      
      // push the new gv file cmd and the full path to the child through to the ipcq reader in gviz.exe process
      int result = mainwind->_ipcqWriter->Push(static_cast<unsigned int>(IpcqCmd::GvFile),mainwind->_gvFullPath);
      assert((result == 0)&&("ShowDefinitionFileDialogCB: ipcq push failed"));

   } // end if

   mainwind->_appStat.tm = mainwind->_tm->GetTmStatus();
   mainwind->_sm->process_event(eSelectTm{mainwind->_appStat});

   return;
} // end ShowDefinitionFileDialogCB

// UserInputStringCB, callback for the _btnLoadString button,
// this reads _inString, course test, and loads it to the _tm and 
// then calls SetupGUITape() to set up the GUI
void MainWind::UserInputStringCB(Fl_Widget *widget, void *param) {

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(param);

   if(mainwind->_appStat.validTm == true) {
      int ret = mainwind->TestInputString();
      if(ret == 0) {
         mainwind->_appStat.validInput = true;
         mainwind->SetInputStringInTM();

         // initialize _tm with the input string 
         mainwind->_tm->Initialize();
         mainwind->_grpTuringTape->SetTapeToB();
         mainwind->SetupGUITape(mainwind->_tm->GetConfigurationType());
      }
      else {
         mainwind->_appStat.validInput = false;
      } // end if 

   } // end if  

   mainwind->_appStat.tm = mainwind->_tm->GetTmStatus();
   mainwind->_sm->process_event(eSetInput{mainwind->_appStat});

   mainwind->_ipcqWriter->Push(static_cast<unsigned int>(IpcqCmd::NewState), mainwind->_tm->GetStartState());

   return; 
} // end UserInputString


// this is the sequencer for the TM, it calls _tm->TransitionStep()
// and then looks at the tm status to decide how to continue or stop 
void MainWind::RunTimerCB(void *data) {

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(data);

   // read here to change rate on the fly  
   mainwind->_runRate = mainwind->GetSequencerRunRate();

   if(mainwind->_appStat.complete == false){

      mainwind->_appStat.looping = true; // set before StepForward()
      mainwind->StepForward();

      Fl::repeat_timeout(mainwind->_runRate, RunTimerCB, data);

   }
   else {

      mainwind->_appStat.looping = true;
      Fl::remove_timeout(RunTimerCB, data);
      
   } // end if 

   return;
} // end RunTimerCB


// RunButton called from the GroupRunControl, this makes sure everything is 
// setup and then it starts the sequncer timer 
void MainWind::RunButton() {
   
   _appStat.tm = _tm->GetTmStatus();
   SetRunStatusBox(_appStat.tm);
   _outStatus->value(RunStateText[static_cast<int>(_appStat.tm)].c_str());

   // set status and start the timer 
   Fl::add_timeout(_runRate, RunTimerCB, this);

   // get the run rate from the dropbox 
   _runRate = GetSequencerRunRate();

   _sm->process_event(eLoop{_appStat});

   return;
} // end RunButton


void MainWind::PauseButton() {

   Fl::remove_timeout(RunTimerCB, this);

   _appStat.tm = _tm->GetTmStatus();
   SetRunStatusBox(_appStat.tm);
   _outStatus->value(RunStateText[static_cast<int>(_appStat.tm)].c_str());

   _sm->process_event(ePause{_appStat});

   return;
} // end PauseButton


void MainWind::StepForward(){

   Transition transition;
   std::string computation;

   transition = _tm->GetCurrentTransition();
   _appStat.complete = false;

   // return 0 success
   // return -1 something went wrong
   // any return sets _status to:
   // TmStatus::InProgress, valid next transition
   // TmStatus::CompleteOnEmptyTransition, no final states
   // TmStatus::AcceptedOnFinalState, if there are final states
   // TmStatus::HaltOnNonFinalState, if there are final states
   // TmStatus::Abnormal_Termination, two - way left on #
   // TmStatus::SomethingWentWrong, should not happen, but ?
   int ret = _tm->TransitionStep();
   _appStat.tm = _tm->GetTmStatus();
   if(0 == ret){

      transition = _tm->GetCurrentTransition();

      switch(_appStat.tm) {
      case TmStatus::InProgress:
   
         if(_appStat.looping != true){
            _sm->process_event(eStepForward{_appStat});
         } // end if 

         break;
      case TmStatus::HaltOnNoTransition:
      case TmStatus::AcceptedOnFinalState:
      case TmStatus::HaltOnNonFinalState:
      case TmStatus::InvalidLeftMove:
      case TmStatus::InvalidRightMove:

         _appStat.looping = false;
         _appStat.complete = true;
         _sm->process_event(eHalt{_appStat});

         break;
      case TmStatus::SomethingWentWrong:  // should not happen 

         _appStat.looping = false;
         _appStat.complete = true;
         _sm->process_event(eHalt{_appStat});

         break;
      case TmStatus::Uninitialized:
         // do nothing 
         break;
      case TmStatus::Ready:
         // do nothing 
         break;
      } // end switch 

   } // end if 

   SetTapeSymbolAndMoveTheHead(transition); 

   // update the computation text and status 
   computation = _tm->GetComputation();
   _bwrComputation->add(computation.c_str());
   SetStatus(_appStat.tm);

   return;
} // end StepForward


void MainWind::StepBackward(){

   Transition transition;
   int result = _tm->StepBack(transition);
   assert((result == 0)&&("StepBackward: tm stepback failed"));
   SetTapeSymbolAndMoveTheHead(transition);

   _bwrComputation->add("Step Backward");
   std::string computation = _tm->GetComputation();
   _bwrComputation->add(computation.c_str());

   _appStat.tm = _tm->GetTmStatus();
   _sm->process_event(eStepBackward{_appStat});

   return;
} // end StepBackward


void MainWind::InitializeTm(){

   // if the TM is not ready like after a run then initialize
   _appStat.tm = _tm->GetTmStatus();
   if(_appStat.tm != TmStatus::Ready) {
      _tm->Initialize();
      _grpTuringTape->SetTapeToB();
      SetupGUITape(_tm->GetConfigurationType());
      _bwrComputation->clear();

      _appStat.complete = false;
      _appStat.looping = false;
      
   } // end if

   _sm->process_event(ePause{_appStat});

   return;
} // end InitializeTm


// ShowSaveComputationFileDialogCB callback for the _btnSaveComputation button,
// this opens a file dialog to let the user select *.txt file to 
// the defintion and computation data into.
// Remark the dialog is preset to save at the user's desktop with 
// a filename that looks like this "computation_<definition_filename>.txt" 
void MainWind::ShowSaveComputationFileDialogCB(Fl_Widget *widget, void *param) {

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(param);

   // setup the file chooser to save the computation text  
   std::string desktop = GetDesktopPath();

   Fl_File_Chooser chooser(desktop.c_str(), "*.txt", 
                           Fl_File_Chooser::CREATE, "Save Computation");
   chooser.ok_label("Save");
   chooser.directory(desktop.c_str());

   std::string fullPath = desktop + "\\" + COMPUTATION_FILE_NAME;
   fullPath = ReplaceSubstring(fullPath, PLACEHOLDER, mainwind->_partialfname);
   chooser.value(fullPath.c_str());

   chooser.show();

   // block until user selects a button
   while(chooser.shown()) Fl::wait();

   if(chooser.value() != NULL) {
      std::string fname(chooser.value());
      mainwind->WriteComputationToFile(fname);
   } // end if 

   return;
} // end ShowSaveComputationFileDialogCB


// return 0 match
// return -1 empty
// return -2 not valid
int MainWind::TestInputString() {
   int ret = 0;

   std::string input(_inString->value());

   StringSet combined = _tm->GetAlphabet();
   StringSet tapeSymbols = _tm->GetTapeSymbols();
   combined.insert(tapeSymbols.begin(), tapeSymbols.end());

   ret = IsStringInAlphabet(combined, input);
   if(ret != 0) {
      _outStatus->value("invalid input string");
      _inString->color(FL_RED);
   }
   else {
      _outStatus->value("valid input string");
      _inString->color(FL_BACKGROUND2_COLOR);
   } // end if 

   return ret;
} // end TestInputString


// SetInputStringInTM, set the input string in the _tm object
// return 0 success
int MainWind::SetInputStringInTM() {
   int ret = 0;

   std::string input(_inString->value());
   _tm->SetInputString(input);

   return ret;
} // end SetInputStringInTM


// returns the definiton file memebr 
std::string MainWind::GetDefinitionfile() {
   return _definitionfile;
} // end GetDefinitionfile


// SetTapeSymbolAndMoveTheHead using transition param
// return 0 on success 
// return -1 on GUI tape limit hit
int MainWind::SetTapeSymbolAndMoveTheHead(Transition transition) {
   int ret = 0;

   int leftIndexValue = _grpTuringTape->GetLeftIndexValue();

   _grpTuringTape->SetSymbolAt(transition.GetIndex() - leftIndexValue, 
                               transition.GetSymbol());

   if(transition.GetDirection() == TAPE_RIGHT_SYMBOL)
      ret = _grpTuringTape->MoveHeadRight();
   else
      ret = _grpTuringTape->MoveHeadLeft();

   _grpTuringTape->SetHeadState(transition.GetState());

   _ipcqWriter->Push(static_cast<unsigned int>(IpcqCmd::NewState),transition.GetState());

   return ret;
} // end SetTapeSymbolAndMoveTheHead


// SetDefinitionfile, set the definiton file member and the 
// _outFilename text box
void MainWind::SetDefinitionfile(const std::string &filename) {
   
   _definitionfile = filename;
   _outFilename->value(filename.c_str());
   _outFilename->position(filename.length());

   return;
} // SetDefinitionfile


// GetSequencerRunRate, return the seconds float value for the 
// selected _icSpeed dropbox value 
// return seconds sequencer rate
float MainWind::GetSequencerRunRate() {
   float ret = 0.0f;
   std::string rateString = _grpRunControls->GetSpeedString();

   auto iter = Rates.find(rateString);
   if(iter != Rates.end()){
      ret = iter->second;
   }
   else {
      assert(false);
   } // end if 

   return ret;
} // end GetSequencerRunRate


// TmFactory, this does the definition file readin and creates the 
// new _tm object. if an error occurs during the readin, _tm 
// is not created and the errors are shown in the status text box 
void MainWind::TmFactory() {

   _appStat.validTm = false;

   // delete the old _tm if it exists  
   if(_tm != nullptr){
      delete _tm;
      _tm = nullptr;
   } // end if 
   
   TuringMachineFactory tmf;
   int ret = tmf.ReadinDefinitionFile(_definitionfile);
   if(ret == 0) {

      _tm = tmf.GetTuringMachine();

      _outDescription->value(_tm->GetDescription().c_str());

      std::string label = "ex: " + _tm->GetInputExample();
      _lbExample->copy_label(label.c_str());
      _lbExample->show();

      _appStat.validTm = true;
      _outStatus->value("valid definition file loaded");

      _inString->value(_tm->GetInputExample().c_str());

      // add the definition to the computation browser
      StringVector definition = _tm->GetDefinition();
      std::for_each(definition.begin(), definition.end(), [&](std::string s) {
                    _bwrComputation->add(s.c_str());
      }); // end of lambda 

   }
   else {
      _outStatus->value(tmf.GetErrorString().c_str());
   } // end if 
   
   return; 
} // end TmFactory


// WriteComputationToFile, called from the 
// ShowSaveComputationFileDialogCB to write the _bwrComputation strings
// to a file from filename (a path)
// return 0 success
// return -1 nothing to write
int MainWind::WriteComputationToFile(const std::string &filename) {
   int ret = 0;

   // return if the browser is empty 
   int count = _bwrComputation->size();
   if(count == 0) return -1;

   // open the file and overwrite if already exists 
   std::ofstream outfile;
   outfile.open(filename.c_str(),std::ofstream::trunc);

   if(outfile.fail()) {
      _outStatus->value("cannot open the computation file for writing");
      return -1;
   } // end if 

   // write the content of the fl_browser to the file 
   // start at index 1, querk of fl_browser 
   for(int i = 1; i <= count; i++) {

      const char *l = _bwrComputation->text(i);
      std::string line(l);

      line += '\n';
      outfile << line;
   } // end for  

   outfile.close();
   _outStatus->value("definition and computations file save to disk");

   return ret;
} // end WriteComputationToFile


// SetRunStatusBox, use the GUI side TmStatus and set 
// the run state box widget label and color 
void MainWind::SetRunStatusBox(TmStatus rs) {

   switch(rs) {
   case TmStatus::HaltOnNoTransition:
      _lbRunState->color(FL_GREEN);
      break;
   case TmStatus::AcceptedOnFinalState:
      _lbRunState->color(FL_GREEN);
      break;
   case TmStatus::HaltOnNonFinalState:
      _lbRunState->color(FL_RED);
      break;
   case TmStatus::Ready:
      _lbRunState->color(FL_BACKGROUND_COLOR);
      break;
   case TmStatus::InProgress:
      _lbRunState->color(FL_BACKGROUND_COLOR);
      break;
   case TmStatus::InvalidLeftMove:
      _lbRunState->color(FL_RED);
      break;
   case TmStatus::InvalidRightMove:
      _lbRunState->color(FL_RED);
      break;
   case TmStatus::SomethingWentWrong:
      _lbRunState->color(FL_RED);
      break;
   case TmStatus::Uninitialized:
      break;
   } // end switch

   _lbRunState->label(RunStateText[static_cast<int>(rs)].c_str());
   _lbRunState->show();

   return;
} // end SetRunStatusBox


void MainWind::SetStatus(TmStatus rs) {

   SetRunStatusBox(rs);
   if(rs != TmStatus::Uninitialized)
      _outStatus->value(RunStateText[static_cast<int>(rs)].c_str());

   return;
} // end SetStatus


// SetupGUITape: setup the GUI tape and head with configuration, 
// standard or two-way
int MainWind::SetupGUITape(TmConfiguration configuration) {
   int ret = -1;

   int idx = 0;
   int positions = _grpTuringTape->GetNumberOfTapePositions();
   int leftIndexValue = _grpTuringTape->GetLeftIndexValue();

   TapeData tapeData;
   bool result = _tm->GetFirstTapePosition(tapeData);
   while(result == true) {

      if(idx < positions) {
         _grpTuringTape->SetSymbolAt(idx - leftIndexValue, std::get<0>(tapeData));
         _grpTuringTape->SetIndexAt(idx - leftIndexValue, std::get<1>(tapeData));
      } // end if 
      
      result = _tm->GetNextTapePosition(tapeData);
      idx++;
   } // end while 

   _grpTuringTape->SetHeadState(_tm->GetState());
   _grpTuringTape->SetInitialHeadPosition();

   return ret;
} // end SetupGUITape

// placed here since _tm header is here in .cpp and  
// only a forward in MainWind.h
TmStatus MainWind::GetTmStatus(){
   return _tm->GetTmStatus();
} // end GetTmStatus






