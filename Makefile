
CXXFLAGS = -std=c++11 -Wall -Wno-strict-aliasing -Wno-unused-variable

#包含头文件路径
SUBDIR   = $(shell ls ./src -R | grep /)
SUBDIRS  = $(subst :,/,$(SUBDIR))
INCPATHS += -I./include
SOURCE = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)*.cpp))

VPATH = $(subst : ,:,$(SUBDIR))./
OBJS = $(patsubst %.cpp,%.o,$(SOURCE))
OBJFILE  = $(foreach dir,$(OBJS),$(notdir $(dir)))
OBJSPATH = $(addprefix obj/,$(OBJFILE)) 

LDFLAGS += `pkg-config --cflags --libs opencv`
LDFLAGS += -pthread  -L/usr/lib/x86_64-linux-gnu -lboost_regex
LDFLAGS += -L./lib -lhcnetsdk -L./lib/HCNetSDKCom -lhpr -lHCCore

EXE = ./ipc

$(EXE): $(SOURCE)
	g++ -std=c++11  -DUSE_BOOST -DRESET_CONFIG -o $(EXE) $(SOURCE)  $(INCPATHS) $(LDFLAGS) -DIPC

.PHONY:clean all
clean:
	rm -rf $(OBJFILE)
	rm -rf $(EXE)

all: $(EXE)
