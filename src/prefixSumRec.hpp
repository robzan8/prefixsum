#ifndef PREFIXSUMREC_HPP
#define PREFIXSUMREC_HPP

#include <ff/parallel_for.hpp>
using namespace ff;

static long min(long x, long y) {
	return x < y ? x : y;
}

template<typename Item>
static void prefixSumRecRec(Item in[], Item out[], Item space[], long len, ParallelFor &pf, long workers) {
	if (len == 1) {
		out[0] = in[0];
		return;
	}

	long len2 = len/2;
	Item *in2 = out; // we can safely use "out" as space to keep "in2"
	Item *out2 = space;

	/*
	The parallel-fors go from 0 to "w", with "i" being the worker id,
	each worker is then assigned a piece of array.
	This has benefits over the canonical way of using the parallel-for:
	- We don't have to call a function closure for each addition operation
		(which introduced non negligible overhead)
	- The sequential for loop inside the parallel-for body
		gives the compiler opportuniny for optimizations and prefetching
	*/
	long w = min(workers, len2);
	pf.parallel_for(0, w, [=](const long i) {
		long first = i * len2 / w;
		long end = (i+1) * len2 / w;

		for (long j = first; j < end; j++) {
    		in2[j] = in[j*2] + in[j*2+1];
    	}
    });

	prefixSumRecRec(in2, out2, space+len2, len2, pf, workers);

	out[0] = in[0];
	long len1 = len - 1;
	w = min(workers, len1);
	pf.parallel_for(0, w, [=](const long i) {
		long first = 1 + i*len1/w;
		long end = 1 + (i+1)*len1/w;

		for (long j = first; j < end; j++) {
			if (j%2 == 0) {
		    	out[j] = out2[j/2-1] + in[j];
		    } else {
		    	out[j] = out2[j/2];
		    }
		}
	});
}

template<typename Item>
void prefixSumRec(Item in[], Item out[], Item space[], long len, long workers) {
	if (len <= 0 || workers <= 0) {
		return;
	}

	ParallelFor pf(min(workers, len), true);
	prefixSumRecRec(in, out, space, len, pf, workers);
}

#endif
