# ref: https://makefiletutorial.com/#running-the-examples

BIN := ./bin

# compiler for c++
CXX := D:/msys64/mingw64/bin/g++.exe
CPPFLAGS := -Wall -std=c++20 -c -MMD -MP \
-ID:/sw_libs/c++/fltk-1.4_mingw64 -ID:/sw_libs/c++/boost_1_78_0 \
-ID:/sw_libs/c++/ipcq/include -ID:/sw_libs/c++/boost_ext

src := $(wildcard *.cpp)
obj := $(src:%.cpp=$(BIN)/%.o)
dep := $(obj:.o=.d)  # one dependency file for each source

LFLAGS = -LD:/sw_libs/c++/fltk-1.4_mingw64/lib -LD:/sw_libs/c++/boost_1_78_0\lib -LD:/sw_libs/c++/ipcq/lib
LIBS = -lfltk -lgdiplus -lcomctl32 -lws2_32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 \
-loleaut32 -luuid -lcomdlg32 -ladvapi32 -lboost_filesystem-mgw11-mt-s-x64-1_78 -lipcq

# the executable to build
TARGET = ../exes/tms.exe

all: $(TARGET)  # first target so run by default if no command line args

debug: CPPFLAGS += -g
debug: $(TARGET)

# build exe
# macros in recipe:  $@ -> target, $^ -> obj 
$(TARGET): $(obj)
	$(CXX) $^ -o $@ $(LFLAGS) $(LIBS)

# compile step for individual source file 
# macros in recipe:  $@ -> target, $< -> first prerequicit 
$(BIN)/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

-include $(dep)   # include all dep files in the makefile

.PHONY: clean
clean:
	cd $(BIN)
	rm -f $(obj) $(dep)
	cd ..

