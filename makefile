# This makefile is based on the one here written by Hiltmon Lipschitz:
# https://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure/

CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=
SRCDIR=src
BUILDDIR=build
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=bin/parth

SRCEXT=cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
LIB=-L lib
INC=-I include

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean