#ifndef BIGINT_MY_VECTOR_H
#define BIGINT_MY_VECTOR_H

#include <algorithm>
#include <cstdint>
#include <vector>

class my_vector {
public:
	uint32_t reference_count;
	std::vector<uint32_t> data;

	my_vector() : reference_count(1) {};
	my_vector(my_vector const& v) : reference_count(1), data(v.data) {};
	explicit my_vector(std::vector<uint32_t> const& v) : reference_count(1), data(v) {};
	my_vector(uint32_t* begin, uint32_t* end) : reference_count(1), data(begin, end) {};

	~my_vector() = default;

	void delete_vector() {
		if (reference_count == 1) {
			delete this;
		} else {
			reference_count--;
		}
	}

};
#endif //BIGINT_MY_VECTOR_H
