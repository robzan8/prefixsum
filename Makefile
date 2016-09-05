CXX=g++
CXXFLAGS=-std=c++11 -O3 -I ./fastflow
LDLIBS= -lpthread
.PHONY: all clear

all: testSerial testRec testSlice

testSerial: ./src/testUtils.cpp
	$(CXX) ./src/testUtils.cpp ./src/testSerial.cpp $(CXXFLAGS) $(LDLIBS) -o ./bin/testSerial

testRec: ./src/testUtils.cpp
	$(CXX) ./src/testUtils.cpp ./src/testRec.cpp $(CXXFLAGS) $(LDLIBS) -o ./bin/testRec

testSlice: ./src/testUtils.cpp
	$(CXX) ./src/testUtils.cpp ./src/testSlice.cpp $(CXXFLAGS) $(LDLIBS) -o ./bin/testSlice

clear:
	rm ./bin/test*
