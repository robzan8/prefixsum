#include <string>
#include <iostream>
using namespace std;
#include "testUtils.hpp"
#include "prefixSum.hpp"
#include <ff/farm.hpp>
#include <pthread.h>

int threads, len, wait;
Double *in, *out;

// prints the array to stdout, if it's short enough
template<typename Item>
static void printArray(Item a[], int len) {
	if (len <= 64) {
		cout << "\n[";
		for (int i = 0; i < len; i++) {
			cout << string(a[i]) << ", ";
		}
		cout << "]\n";
	}
}

void *doPrefixSum(void *foo) {
	Double::wait = wait;
	double start = millisNow();
	prefixSum(in, out, len);
	double end = millisNow();
	cout << string(Double(end-start)); 
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	string initMode;
	parseArgs(argc, argv, threads, len, wait, initMode);

	in = new Double[len];
	out = new Double[len];
	if (in == NULL || out == NULL) {
		cerr << "Error allocating memory.\n";
		return 1;
	}
	initNumericArray(in, len, initMode);

	pthread_t thread;
	pthread_create(&thread, NULL, doPrefixSum, NULL);
	pthread_join(thread, NULL);

	// printArray(out, len);

	delete[] in;
	delete[] out;
	return 0;
}
