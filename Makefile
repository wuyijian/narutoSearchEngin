INC_DIR:=./net/ ./net/inc/ ./base/inc/
SRC_DIR:=./base/ ./base/src/ ./base/src/
SRCS:=$(wildcard ./net/*.cc) $(wildcard ./net/src/*.cc) $(wildcard ./net/src/*.c) $(wildcard ./base/src/*.cc)
LIBS:= -llog4cpp -ljson -lhiredis -lpthread

CXX:=clang++

CXXFLAGS:= -Wall -g -std=c++11 $(addprefix -I, $(INC_DIR)) $(LIBS) -Wno-deprecated

EXE:=./bin/main

$(EXE):$(SRCS)
	$(CXX) -o $(EXE) $(SRCS) $(CXXFLAGS)

clean:
	rm -rf $(EXE)
