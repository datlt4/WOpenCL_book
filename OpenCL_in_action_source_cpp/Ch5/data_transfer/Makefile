# Compiler and flags
CXX=g++
CC=gcc
CFLAGS=-std=c11

# Project names
PROJ=data_transfer
PROJ_CPP=$(PROJ).cpp_app
PROJ_C=$(PROJ).c_app

# Detect OS
UNAME_S := $(shell uname -s)

# Architecture detection
ARCH := $(shell uname -m)

# Include and library directories and libraries
ifeq ($(UNAME_S), Linux)
    CXXFLAGS=-std=c++2a
    INC_DIRS=-I /usr/local/cuda/include
    LIB_DIRS=-L /usr/local/cuda/lib64
    LIBS=-lOpenCL
    ifeq ($(ARCH), aarch64)
        DEFINES=-DCL_VERSION_2_x
    else ifeq ($(ARCH), x86_64)
        DEFINES=-DCL_VERSION_3_x
    endif
else ifeq ($(UNAME_S), Darwin)
    CXXFLAGS=-std=c++20
    INC_DIRS=
    LIB_DIRS=
    LIBS=-framework OpenCL
    DEFINES=-DAPPLE
endif

# Default target to build both applications
all: $(PROJ_CPP) $(PROJ_C)

# Rules for building the C++ application
$(PROJ_CPP): $(PROJ).cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LIBS) $(INC_DIRS) $(LIB_DIRS) $(DEFINES)

# Rules for building the C application
$(PROJ_C): $(PROJ).c
	$(CC) $^ -o $@ $(CFLAGS) $(LIBS) $(INC_DIRS) $(LIB_DIRS) $(DEFINES)

.PHONY: clean

# Clean rule
clean:
	rm -f $(PROJ_CPP) $(PROJ_C)
