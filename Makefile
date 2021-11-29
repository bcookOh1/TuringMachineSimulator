
BIN := ./bin

# compiler for c++
CXX := D:/msys64/mingw64/bin/g++.exe
CPPFLAGS := -Wall -std=c++20 -MMD -MP -ID:/sw_libs/c++/fltk-1.4_mingw64 -ID:/sw_libs/c++/boost_1_77_0

src := $(wildcard *.cpp)
obj := $(src:.cpp=.o)
dep := $(obj:.o=.d)  # one dependency file for each source

LFLAGS = -LD:/sw_libs/c++/fltk-1.4_mingw64/lib 
LIBS = -lfltk -lgdiplus -lcomctl32 -lws2_32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32

# the executable to build
TARGET = tms.exe

all: $(TARGET)  # first target so run by default if no command line args

debug: CPPFLAGS += -g
debug: $(TARGET)

# macros in recipe:  $@ -> target, $^ -> obj 
$(TARGET): $(obj)
	$(CXX) -o $@ $^ $(LFLAGS) $(LIBS)

-include $(dep)   # include all dep files in the makefile

.PHONY: clean
clean:
	#cd $(BIN)
	rm -f $(obj) $(dep)
	#cd ..

