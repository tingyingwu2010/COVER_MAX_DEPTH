# COMPILER

CXX= g++ 

# FILES

OBJS =	src/Main.o src/global_variables.o src/global_functions.o src/DP.o

DBG= -O3
#DBG= -g

CXXFLAGS =  $(DBG)

.c.o:
	gcc -c $(CXXFLAGS) $< -o $@


all:RATIO

RATIO: $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) 

$(OBJS): Makefile

clean:
	rm -f $(OBJS) rm RATIO

