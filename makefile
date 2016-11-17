TARGET=target
CC=g++
CPPFLAGS=
ODIR=obj
DEPDIR=dep
SRCDIR=src
INCDIR=inc
MAKEFILE=makefile
INCLUDES:=$(wildcard $(INCDIR)/*.hpp)
CPPSRC:=$(wildcard $(SRCDIR)/*.cpp)
OBJS:=$(patsubst $(SRCDIR)/%.cpp,$(ODIR)/%.o,$(CPPSRC))
DEPS:=$(patsubst $(SRCDIR)/%.cpp,$(DEPDIR)/%.d,$(CPPSRC))

#include --static and the -static in the library to link statically
#remove these to link with the shared libraries dyamically
LIBS=
CFLAGS=

#rule for a target
CURRENT_TARGET=$(patsubst $(ODIR)/%.o, $(SRCDIR)/%.cpp, $@)

#default target
all: $(TARGET) 

include $(DEPS) 

#specify rule for each object file
#each object file is dependent on all src and header files
#even if not included, just to make things simpler for now
$(OBJS): 
	$(CC) -Wall -I$(INCDIR) -c $(CURRENT_TARGET) -o $@ $(CFLAGS) $(LIBS)

#link all object files to create executable
$(TARGET): $(OBJS)
	@echo
	$(CC) $(OBJS) -o $@ $(LIBS) $(CFLAGS)

#in bash, $$ means expand to shell process ID.
#This guarantees uniqueness of a file name
$(DEPDIR)/%.d: $(SRCDIR)/%.cpp
	@echo "Building dependencies for $@"
	@set -e; rm -f $@; \
	$(CC) -MM -MT $(ODIR)/$*.o $(CPPFLAGS) -I$(INCDIR) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

print_vars:
	@echo
	@echo $(OBJS)
	@echo $(SRCDIR)
	@echo $(CPPSRC)
	@echo $(INCLUDES)
	@echo $(SRCDIR)/%.cpp
	@echo $(DEPS)
	@echo

gen_tags:
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+qf .
	cscope -R -b

.PHONY: clean
clean: 
	rm -f $(TARGET) \
	$(OBJS) \
	$(DEPS);

#The vpath directive only controls how Make finds dependencies; it doesn't affect in any way how GCC works. If you have headers in some other directory, you explicitly need to tell GCC with -I:
#INCLUDE := include
#$(CC) -I$(INCLUDE) $c $< -o $@
