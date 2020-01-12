FORCE:
.PHONY: FORCE

SUFFIX   	= .cpp
SRCDIR   	= ./src
OBJDIR   	= ./obj
TESTDIR  	= ./test
SRCS  		= $(wildcard $(SRCDIR)/*$(SUFFIX))
OBJS  		= $(SRCS:$(SRCDIR)%$(SUFFIX)=$(OBJDIR)%.o)
TESTS 		= $(wildcard $(TESTDIR)/*$(SUFFIX))
TARGETS 	= $(basename $(TESTS))

obj/%.o: src/%.cpp
	g++ --std=c++17 -c ./src/$*.cpp -o ./obj/$*.o

$(TESTS): $(OBJS) FORCE
	@g++ --std=c++17 $@ ./obj/*.o
	@./a.out "test.tas" "enemy001"
	@echo "$@ <<< OK"

test: $(TESTS)
	@g++ --std=c++17 test.cpp obj/*.o
	@./a.out

library: $(OBJS)
	ar r lib/libtombeeactionscript.a obj/*.o

clean:
	rm -rf a.out
