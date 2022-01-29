
#include "GraphvizUtil.h"
#include "MainWind.h"



// bcook 12-08-2021
int WriteGraphvizDotFile(TuringMachine *tm, std::string tmFilename, std::string &gvFullPath) {
   int ret = 0;

   std::set<string> stateList;
   map<string, string> stateToStateTransition;

   // TmTransitionTable::iterator tableIter;
   for(auto tableIter = tm->_transitionTable.begin(); tableIter != tm->_transitionTable.end(); ++tableIter) {
      for(auto rowIter = tableIter->second.begin(); rowIter != tableIter->second.end(); ++rowIter) {

         string fromState = tableIter->first;
         string toState = rowIter->second.GetState();
         string readSymbol = rowIter->first;
         string writeSymbol = rowIter->second.GetSymbol();
         string direction = rowIter->second.GetDirection();

         // add both from and to states, duplicates ignored
         stateList.insert({ fromState });
         stateList.insert({ toState });

         string stateToState = fromState + " -> " + toState;
         string transition = readSymbol + "/" + writeSymbol + " " + direction;
         auto found = stateToStateTransition.find(stateToState);
         if(found != stateToStateTransition.end()) {
            found->second += "\\n" + transition;
         }
         else {
            stateToStateTransition.emplace(make_pair(stateToState, transition));
         } // end if 

      } // end for
   } // end for 

   // create a graphviz dot file folder
   fs::path graphviz_folder{"img"};
   try {
      fs::create_directory(absolute(graphviz_folder));
   }
   catch(fs::filesystem_error &e) {
      cerr << e.what() << '\n';
      return -1;
   } // end try/catch

   // make a graphviz file name
   fs::path fullpath = fs::current_path();
   fullpath /= graphviz_folder;
   fullpath /= string(tmFilename + ".gv");

   // open the file and overwrite if already exists 
   ofstream outfile;
   outfile.open(fullpath.c_str(), ofstream::trunc);
   if(outfile.fail()) {
      return -2;
   } // end if 

   // copy the gv (dot format) path to the parameter  
   gvFullPath = fullpath.string();

   outfile << "digraph { \n // dot -Tsvg gvFilename.gv -o gvImage.svg" << endl;
   
   // add a coma separated state list, uses to add a state ellipse color
   ostringstream oss;
   oss << StatesCommentPrefex;
   for_each(begin(stateList), end(stateList), [&](string item) {oss << item << ","; });
   string stateLine = oss.str();
   stateLine.pop_back();

   outfile << stateLine << endl;
   outfile << "   rankdir = LR" << endl;

   for_each(begin(stateList), end(stateList), [&](string item) {outfile << "   " << item << BlackAttribStr << endl; });
   for_each(begin(stateToStateTransition), end(stateToStateTransition), [&](auto const &mpair) {
      outfile << "   " << mpair.first << " [label=\"" << mpair.second << "\"]; "<< endl; }
   );

   outfile << "}" << endl;
   outfile.close();

   return ret;
} // end WriteGraphvizDotFile

