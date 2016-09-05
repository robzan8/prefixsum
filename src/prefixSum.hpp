#ifndef PREFIXSUM_HPP
#define PREFIXSUM_HPP

template<typename Item>
void prefixSum(Item in[], Item out[], long len) {
	if (len <= 0) {
		return;
	}
	
	out[0] = in[0];
	for (long i = 1; i < len; i++) {
		out[i] = out[i-1] + in[i];
	}
}

#endif
