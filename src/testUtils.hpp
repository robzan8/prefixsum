#ifndef TESTUTILS_HPP
#define TESTUTILS_HPP

#include <stdio.h>
#include <string>
using namespace std;

void parseArgs(int argc, char *argv[], int &threads, int &len, int &wait, string &initMode);

double millisNow();

class Double {
public:
	double x;
	static int wait; // active wait of sum operations

	Double(double d) {
		x = d;
	}
	Double(int i) {
		x = i;
	}
	Double() {
		x = 0;
	}

	operator string() {
		char buffer[32];
		snprintf(buffer, sizeof(buffer), "%.2f", x);
		return string(buffer);
	}
	Double operator +(Double d) {
		volatile long foo = 0;
		for (long i = 0; i < wait; i++) {
			foo += i;
		}
		return Double(x + d.x);
	}
	bool operator ==(Double d) {
		return x == d.x;
	}
};

template<typename Number>
void initNumericArray(Number a[], int len, string initMode) {
	if (initMode.compare("increasing") == 0) {
		for (int i = 0; i < len; i++) {
			a[i] = i;
		}
		return;
	}

	if (initMode.compare("decreasing") == 0) {
		for (int i = 0; i < len; i++) {
			a[i] = len - i;
		}
		return;
	}

	// default initMode: ones
	for (int i = 0; i < len; i++) {
		a[i] = 1;
	}
}

template<typename Item>
bool equal(Item a[], Item b[], int len) {
	for (int i = 0; i < len; i++) {
		if (!(a[i] == b[i])) {
			return false;
		}
	}
	return true;
}

#endif
