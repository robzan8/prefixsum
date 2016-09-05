#ifndef PREFIXSUMSLICE_HPP
#define PREFIXSUMSLICE_HPP

#include <vector>
using namespace std;
#include "prefixSum.hpp"
#include <ff/farm.hpp>
using namespace ff;

#define PF_CACHELINE 64

template<typename Item>
class Task {
	public:
	Item *in, *out, high;
	long len, num, round;

	Task(Item *i, Item *o, long l, long n) {
		in = i; out = o; len = l; num = n;
		round = 0;
		/*
		round is initially 0 and is set to 1 when the tasks
		are emitted again through the feedback channel.
		This allows the workers and the collector to understand
		if we are in step 1 or step 3 of the algorithm.
		*/
	}
	void work() { // called on each task by the workers
		if (round == 0) {
			prefixSum(in, out, len);
			high = out[len-1];
		} else if (round == 1) {
			for (long i = 0; i < len; i++) {
				out[i] = out[i] + high;
			}
		}
	}
};

template<typename Item>
class Emitter: public ff_node {
	private:
	Item *in, *out;
	long len, slices;
	Task<Item> *tasks;

	public:
	Emitter(Item *i, Item *o, long l, long s) {
		in = i; out = o; len = l; slices = s;
		tasks = (Task<Item> *)calloc(s, sizeof(Task<Item>));
	}

	void *svc(void *task) {
		if (task == NULL) {
			return initialEmit();
		}
		return task;
	}

	void *initialEmit() {
		long line;
		long size = sizeof(Item) * len;
		if ((long)in%PF_CACHELINE == 0 && sizeof(Item) < PF_CACHELINE &&
			PF_CACHELINE%sizeof(Item) == 0 && size >= PF_CACHELINE*slices) {
			// if possible, align slices to cache line (prevents false sharing)
			line = PF_CACHELINE;
		} else {
			// otherwise, align slices to item size
			line = sizeof(Item);
		}
		long lines = size / line;
		if (size%line != 0) {
			lines++;
		}
		long itemsPerLine = line/sizeof(Item);
		// slice the arrays properly and send out tasks
		for (long i = 0; i < slices; i++) {
			long first = (i*lines/slices) * itemsPerLine;
			long end = ((i+1)*lines/slices) * itemsPerLine;
			if (end > len) {
				end = len;
			}
			tasks[i] = Task<Item>(in+first, out+first, end-first, i);
			ff_send_out(&tasks[i]);
		}
		return GO_ON;
	}

	~Emitter() {
		free(tasks);
	}
};

template<typename Item>
class Worker: public ff_node {
	public:
	void *svc(void *task) {
		((Task<Item> *)task)->work();
		return task;
	}
};

template<typename Item>
class Collector: public ff_node {
	private:
	long slices, received;
	Task<Item> *tasks;

	public:
	Collector(long s) {
		slices = s;
		received = 0; // counts how many tasks have been received
		tasks = (Task<Item> *)calloc(s, sizeof(Task<Item>));
	}

	void *svc(void *task) {
		Task<Item> *t = (Task<Item> *)task;
		if (t->round == 0) {
			tasks[t->num] = *t; // store tasks in original order
			if (++received == slices) { // end of step 1
				return emitFeedback();
			}
		} else if (t->round == 1) {
			if (++received == slices) { // end of step 3
				return EOS;
			}
		}
		return GO_ON;
	}

	void *emitFeedback() {
		if (slices == 1) {
			return EOS;
		}
		long slices1 = slices - 1;
		Item highIn[slices1];
		for (long i = 0; i < slices1; i++) {
			highIn[i] = tasks[i].high;
		}
		Item highOut[slices1];
		prefixSum(highIn, highOut, slices1);

		received = 0;
		tasks[0].round = 1;
		tasks[0].len = 0; // we don't have to compute anything for slice 0 in step 3
		ff_send_out(&tasks[0]);
		for (long i = 1; i < slices; i++) {
			tasks[i].round = 1;
			tasks[i].high = highOut[i-1];
			ff_send_out(&tasks[i]);
		}
		return GO_ON;
	}

	~Collector() {
		free(tasks);
	}
};

template<typename Item>
void prefixSumSlice(Item in[], Item out[], long len, long workers) {
	if (len <= 0 || workers <= 0) {
		return;
	}
	if (len < workers) {
		workers = len;
	}

	ff_farm<> farm;
	Emitter<Item> emitter(in, out, len, workers);
	farm.add_emitter(&emitter);
	Collector<Item> collector(workers);
	farm.add_collector(&collector);

	Worker<Item> wArray[workers];
	vector<ff_node *> wVec(workers);
	for (long i = 0; i < workers; i++) {
		wVec[i] = &wArray[i];
	}
	farm.add_workers(wVec);

	farm.wrap_around();
	farm.run_and_wait_end();
}

#endif
