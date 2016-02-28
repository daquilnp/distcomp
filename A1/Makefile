# Used from a personal project, tailored to 4F03
# Original purpose: Makefile to build TREK, the packing list expert system

CC=mpicc #gcc
RM=rm -f
CFLAGS=-I. #-std=c++0x #-std=gnu++0x #-Wall -Weffc++

FINAL_EXECUTABLE_NAME=ppmf

SRCS=main.cpp readwriteppm.cpp filter.cpp sort.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: ppmf

ppmf: $(OBJS) 
	$(CC) $(CFLAGS) -o $(FINAL_EXECUTABLE_NAME) $(OBJS) $(LDLIBS) 
clean: dist-clean
	$(RM) $(OBJS)
dist-clean:
	$(RM) $(FINAL_EXECUTABLE_NAME)

