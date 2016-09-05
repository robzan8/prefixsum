#include <string>
#include <iostream>
using namespace std;
#include "testUtils.hpp"
#include "prefixSum.hpp"
#include "prefixSumSlice.hpp"

int main(int argc, char *argv[]) {
	int threads, len, wait;
	string initMode;
	parseArgs(argc, argv, threads, len, wait, initMode);

	Double *in, *parallelOut;
	int err1 = posix_memalign((void **)&in, PF_CACHELINE, sizeof(Double)*len);
	int err2 = posix_memalign((void **)&parallelOut, PF_CACHELINE, sizeof(Double)*len);
	if (err1 || err2) {
		cerr << "Error allocating memory.\n";
		return -1;
	}
	initNumericArray(in, len, initMode);

	Double::wait = wait;
	double start = millisNow();
	prefixSumSlice(in, parallelOut, len, threads);
	double end = millisNow();
	cout << string(Double(end-start));
	
	/*
	// uncomment this block if you want to check algorithm correctness
	// (slows down benchmarking)
	Double *serialOut = new Double[len];
	prefixSum(in, serialOut, len);
	if (!equal(serialOut, parallelOut, len)) {
		cerr << "Error: serial and parallel results don't match :(\n";
		return -2;
	}
	delete[] serialOut;
	*/

	free(in);
	free(parallelOut);
	return 0;
}
