CC=g++
LD=g++

EXEC=parth
CPPFLAGS=-isystem $(GTEST_DIR)/include
CXXFLAGS=-Wall -Wextra -std=c++11 -pthread
LDFLAGS=
LIB=-L lib
INC=-I include

#*** GTEST ***#

GTEST_DIR = lib/googletest
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

#*** DIRECTORIES ***#

SRCDIR=src
BUILDDIR=build
TESTDIR=test
BINDIR=bin
LIBDIR=lib

#*** TARGETS ***#

TARGET=bin/parth
TESTTARGET=bin/runTest

#*** WORKING FILE LOCATIONS ***#

SOURCES := $(shell find $(SRCDIR) -type f -name *.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))

#*** PROJECT DEPENDENCIES ***#

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(GTEST_HEADERS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(INC) -c -o $@ $<

#*** GTEST DEPENDENCIES ***#
# Could be a bit more optimized

GTEST_DEPS=gtest-all.o gtest_main.o
GTEST_OBJS=$(patsubst %,$(BUILDDIR)/%,$(GTEST_DEPS))

build/gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc -o $@

build/gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc -o $@

lib/gtest.a : $(BUILDDIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

lib/gtest_main.a : $(BUILDDIR)/gtest-all.o $(BUILDDIR)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

#*** TESTS ***#

TESTSOURCES := $(shell find $(TESTDIR) -type f -name *_test.cpp)
TESTOBJECTS := $(patsubst $(TESTDIR)/%,$(BUILDDIR)/%,$(TESTSOURCES:.cpp=.o))

.PHONY: tests

tests: $(TESTTARGET)
	bin/runTest

$(TESTTARGET): $(TESTOBJECTS) $(filter-out build/main.o,$(OBJECTS)) lib/gtest_main.a
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

$(BUILDDIR)/%_test.o: $(TESTDIR)/%_test.cpp $(GTEST_HEADERS)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(INC) -c $< -o $@

#*** CLEAN ***#

clean:
	rm -rfv $(BUILDDIR)/* $(TARGET) $(TESTTARGET)

.PHONY: clean