FORCE:
.PHONY: FORCE all test library clean vscode

SUFFIX   	= .cpp
SRCDIR   	= ./src
OBJDIR   	= ./obj
TESTDIR  	= ./test
LIBDIR   	= ./lib
SRCS  		= $(wildcard $(SRCDIR)/*$(SUFFIX))
OBJS  		= $(SRCS:$(SRCDIR)%$(SUFFIX)=$(OBJDIR)%.o)
TESTS 		= $(wildcard $(TESTDIR)/*$(SUFFIX))

all: $(OBJS) library

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	g++ --std=c++17 -c $< -o $@

$(TESTS): $(OBJS) FORCE | $(OBJDIR) $(LIBDIR)
	@g++ --std=c++17 $@ $(OBJS) -o a.out
	@./a.out "test.tas" "enemy001"
	@echo "$@ <<< OK"
	@rm -f a.out

test: $(TESTS)

library: $(OBJS) | $(LIBDIR)
	ar r $(LIBDIR)/libtombeeactionscript.a $(OBJS)

vscode:
	mkdir -p $(HOME)/.vscode/extensions
	ln -sfn $(CURDIR)/editors/vscode $(HOME)/.vscode/extensions/tas-vscode
	@echo "TAS VS Code extension installed to ~/.vscode/extensions/tas-vscode"

clean:
	rm -rf a.out $(OBJDIR) $(LIBDIR)

