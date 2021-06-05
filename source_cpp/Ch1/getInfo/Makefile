SRCS= getInfo.h
OBJS=getInfo.o
EXEC=getInfo

GCC ?= g++
CCFLAGS := -std=c++11

LDFLAGS += -ggdb3 -O2 -Wall -Wextra -pedantic -lOpenCL

RM=rm -f

# Target rules
all: build

build: ${EXEC}

%.o: %.cpp %.h
	$(GCC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

${EXEC}: ${OBJS}
	$(GCC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	${RM} ${EXEC} *.o 

dist-clean:
	$(RM) $(EXEC)

