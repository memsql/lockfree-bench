CXX= g++ -std=c++0x
CXXFLAGS= -W -Wall -O2 -pthread

# For OS X, uncomment the next line:
#CXX= clang++ -std=c++11 -stdlib=libc++

all: benchmark

benchmark: benchmark.cpp
	$(CXX) $(CXXFLAGS) benchmark.cpp -o benchmark
