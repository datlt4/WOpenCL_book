PROJ=getInfo
CXX = g++
CXXFLAGS=-std=c++11
INC_DIRS=-I /usr/local/cuda/include
LIB_DIRS=-L /usr/local/cuda/lib64/libOpenCL.so

LIBS=-lOpenCL

$(PROJ): $(PROJ).cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LIBS) $(INC_DIRS) $(LIB_DIRS)

.PHONY: clean

clean:
	rm -f $(PROJ)
