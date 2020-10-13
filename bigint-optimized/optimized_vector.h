#ifndef BIGINT_OPTIMIZED_VECTOR_H
#define BIGINT_OPTIMIZED_VECTOR_H

#include <algorithm>
#include "my_vector.h"

class optimized_vector {
public:
	optimized_vector() : is_small_(true), size_(0) {};

	optimized_vector(optimized_vector const& other) : is_small_(other.is_small_), size_(other.size_) {
		if (is_small_) {
			std::copy_n(other.static_vec, size_, static_vec);
		} else {
			other.reference_counter()++;
			dynamic_vec = other.dynamic_vec;
		}
	};

	optimized_vector& operator=(optimized_vector const& other) {
		if (&other != this) {
			optimized_vector safe(other);
			swap(safe);
		}
		return *this;
	}

	~optimized_vector() {
		if (!is_small_) {
			dynamic_vec->delete_vector();
		}
	}

	bool empty() const {
		return size_ == 0;
	}

	size_t size() const {
		return size_;
	}

	size_t capacity() const {
		return is_small_ ? SMALL_SZ : dynamic_vec->data.capacity();
	}

	uint32_t const& operator[](size_t i) const {
		return is_small_ ? static_vec[i] : dynamic_vec->data[i];
	}

	uint32_t const& back() const {
		return (*this)[size_ - 1];
	}

	uint32_t& operator[](size_t i) {
		make_unique();
		return is_small_ ? static_vec[i] : dynamic_vec->data[i];
	}

	uint32_t& back() {
		make_unique();
		return (*this)[size_ - 1];
	}

	void push_back(uint32_t x) {
		make_unique();
		if (size_ == SMALL_SZ) {
			make_big();
		}
		if (is_small_) {
			static_vec[size_] = x;
		} else {
			dynamic_vec->data.push_back(x);
		}
		size_++;
	}

	void pop_back() {
		make_unique();
		if (!is_small_) {
			dynamic_vec->data.pop_back();
		}
		size_--;
	}

	uint32_t const* begin() const {
		return is_small_ ? static_vec : dynamic_vec->data.data();
	}

	uint32_t* begin() {
		make_unique();
		return is_small_ ? static_vec : dynamic_vec->data.data();
	}

	uint32_t const* end() const {
		return begin() + size_;
	}

	uint32_t* end() {
		return begin() + size_;
	}

	void insert(uint32_t* begin_, size_t count, uint32_t x) {
		make_unique();
		if (size_ + count > SMALL_SZ) {
			make_big();
			dynamic_vec->data.insert(dynamic_vec->data.begin() + (begin_ - begin()), count, x);
		} else {
			for (uint32_t* it = end() - 1; it >= begin_ + count; it--) {
				*it = *(it - count);
			}
			for (uint32_t* it = begin_; it != begin_ + count; it++) {
				*it = x;
			}
		}
		size_ += count;
	}


	void erase(uint32_t* begin_, uint32_t* end_) {
		make_unique();
		ptrdiff_t count = end_ - begin_;
		if (!is_small_) {
			dynamic_vec->data.erase(dynamic_vec->data.begin() + (begin_ - begin()),
				dynamic_vec->data.begin() + (end_ - begin()));
		} else {
			for (uint32_t* it = begin_; it + count != end(); it++) {
				*it = *(it + count);
			}
		}
		size_ -= count;
	}

private:
	static constexpr size_t SMALL_SZ = 2;
	bool is_small_ = true;
	size_t size_ = 0;
	union {
		uint32_t static_vec[SMALL_SZ];
		my_vector* dynamic_vec;
	};

	uint32_t& reference_counter() const {
		return dynamic_vec->reference_count;
	}

	void swap_small_big(optimized_vector& other) {
		my_vector* old_vector = other.dynamic_vec;
		std::copy_n(static_vec, size_, other.static_vec);
		dynamic_vec = old_vector;
	}

	void swap(optimized_vector& other) {
		if (is_small_ && other.is_small_) {
			std::swap(static_vec, other.static_vec);
		} else if (!is_small_ && !other.is_small_) {
			std::swap(dynamic_vec, other.dynamic_vec);
		} else if (is_small_) {
			swap_small_big(other);
		} else {
			other.swap_small_big(*this);
		}
		std::swap(size_, other.size_);
		std::swap(is_small_, other.is_small_);
	}

	void make_unique() {
		if (!is_small_ && reference_counter() > 1) {
			reference_counter()--;
			dynamic_vec = new my_vector(*dynamic_vec);
		}
	}

	void make_big() {
		if (is_small_) {
			dynamic_vec = new my_vector(static_vec, static_vec + size_);
			is_small_ = false;
		}
	}

};
#endif //BIGINT_OPTIMIZED_VECTOR_H
