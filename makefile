CC=g++	 # linker
CXX=g++  # compiler
CPPFLAGS=-std=c++11
LDLIBS=-lboost_program_options 
SHELL=bash

.PHONY: clean all
bins := compact similarity predict

all: $(bins)

similarity: similarity.o util.o

predict: predict.o util.o

clean: 
	rm -f $(bins) *.o *.d


# header files

sources := $(shell ls *.cpp)
-include $(sources:.cpp=.d)
%.d: %.cpp
	@set -e; rm -f $@; \
    $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$

