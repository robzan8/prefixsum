#include <string>
#include <iostream>
using namespace std;
#include "testUtils.hpp"
#include "prefixSum.hpp"
#include "prefixSumRec.hpp"

int main(int argc, char *argv[]) {
	int threads, len, wait;
	string initMode;
	parseArgs(argc, argv, threads, len, wait, initMode);

	Double *in = new Double[len];
	Double *parallelOut = new Double[len];
	Double *space = new Double[len];
	if (in == NULL || parallelOut == NULL || space == NULL) {
		cerr << "Error allocating memory.\n";
		return -1;
	}
	initNumericArray(in, len, initMode);

	Double::wait = wait;
	double start = millisNow();
	prefixSumRec(in, parallelOut, space, len, threads);
	double end = millisNow();
	cout << string(Double(end-start));
	
	/*
	// uncomment this block if you want to check algorithm correctness
	// (slows down benchmarking)
	Double *serialOut = space;
	prefixSum(in, serialOut, len);
	if (!equal(serialOut, parallelOut, len)) {
		cerr << "Error: serial and parallel results don't match :(\n";
		return -2;
	}
	*/

	delete[] in;
	delete[] parallelOut;
	delete[] space;
	return 0;
}
