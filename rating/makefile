CC=g++	 # linker
CXX=g++  # compiler
CPPFLAGS=-std=c++11

rating: rating.o linear.o
	$(CC) -lboost_program_options  $^ -o $@


clean: 
	rm -f *.o rating *.d



# header files

sources := $(shell ls *.cpp)
-include $(sources:.cpp=.d)
%.d: %.cpp
	@set -e; rm -f $@; \
    $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$

