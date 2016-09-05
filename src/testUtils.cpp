#include <stdlib.h>
#include <time.h>
#include <string>
using namespace std;
#include "testUtils.hpp"

int Double::wait = 0;

void parseArgs(int argc, char *argv[], int &threads, int &len, int &wait, string &initMode) {
	if (argc >= 2) {
		threads = atoi(argv[1]);
	} else {
		threads = 2;
	}

	if (argc >= 3) {
		len = atoi(argv[2]);
	} else {
		len = 1000000;
	}

	if (argc >= 4) {
		wait = atoi(argv[3]);
	} else {
		wait = 3000;
	}

	if (argc >= 5) {
		initMode = string(argv[4]);
	} else {
		initMode = "once";
	}
}

double millisNow() {
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return double(now.tv_sec)*1000.0 + double(now.tv_nsec)/1000000.0;
}
