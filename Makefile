-Wall == all warnings enabled
-Werror == treat warnings as ERRORS!
CPPFLAGS = --std=c++17 -Wall -DBOOST_TEST_DYN_LINK
LINKFLAGS = --std=c++17 -lboost_unit_test_framework

lib_SOURCES = \
    include/TreeMap.h \
	include/HashMap.h

lib_OBJECTS=$(lib_SOURCES:.cpp=.o)

tests_SOURCES = \
	tests/tests_main.cpp \
    tests/TreeMapTests.cpp \
	tests/HashMapTests.cpp
	

tests_OBJECTS=$(tests_SOURCES:.cpp=.o)

tests_EXECUTABLE = tests_bin

all: tests

%.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(tests_EXECUTABLE): $(tests_OBJECTS) $(lib_OBJECTS)
	$(CXX) -o $@ $(tests_OBJECTS) $(lib_OBJECTS) $(LINKFLAGS)

tests: $(tests_EXECUTABLE)
	./$(tests_EXECUTABLE)

.PHONY: clean tests

clean:
	- rm tests/*.o
	- rm tests_bin
	- rm profile