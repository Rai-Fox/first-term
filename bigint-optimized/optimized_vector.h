#ifndef BIGINT_OPTIMIZED_VECTOR_H
#define BIGINT_OPTIMIZED_VECTOR_H

#include <algorithm>
#include "my_vector.h"

struct optimized_vector {
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

	void swap(optimized_vector& other) {
		if (is_small_) {
			size_t old_size = size_;
			uint32_t old_static_vec[SMALL_SZ];
			std::copy_n(static_vec, size_, old_static_vec);
			if (other.is_small_) {
				std::copy_n(other.static_vec, other.size_, static_vec);
			} else {
				dynamic_vec = other.dynamic_vec;
			}
			std::copy_n(old_static_vec, old_size, other.static_vec);
		} else {
			if (other.is_small_) {
				my_vector* old_dynamic_vec = dynamic_vec;
				std::copy_n(other.static_vec, other.size_, static_vec);
				other.dynamic_vec = old_dynamic_vec;
			} else {
				std::swap(other.dynamic_vec, dynamic_vec);
			}
		}
		std::swap(size_, other.size_);
		std::swap(is_small_, other.is_small_);
	}

	void make_unique() {
		if (!is_small_ && reference_counter() > 1) {
			this->~optimized_vector();
			dynamic_vec = new my_vector(*dynamic_vec);
			reference_counter() = 1;
		}
	}

	void make_big() {
		if (is_small_) {
			dynamic_vec = new my_vector(static_vec, static_vec + size_);
			is_small_ = false;
		}
	}

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
		if (size_ >= SMALL_SZ) {
			make_big();
			dynamic_vec->data.push_back(x);
		} else {
			static_vec[size_] = x;
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
		return is_small_ ? static_vec : &*(dynamic_vec->data.begin());
	}

	uint32_t* begin() {
		make_unique();
		return is_small_ ? static_vec : &*(dynamic_vec->data.begin());
	}

	uint32_t const* end() const {
		return begin() + size_;
	}

	uint32_t* end() {
		make_unique();
		return begin() + size_;
	}

	void insert(uint32_t* begin_, size_t count, uint32_t x) {
		make_unique();
		if (size_ + count > SMALL_SZ) {
			make_big();
			dynamic_vec->data.insert(dynamic_vec->data.begin() + (begin_ - begin()), count, x);
		}
		size_ += count;
	}

	void erase(uint32_t* begin_, uint32_t* end_) {
		make_unique();
		if (!is_small_) {
			dynamic_vec->data.erase(dynamic_vec->data.begin() + (begin_ - begin()),
				dynamic_vec->data.begin() + (end_ - begin()));
		}
		size_ -= std::min(size_, static_cast<size_t>(end_ - begin_));
	}
};
#endif //BIGINT_OPTIMIZED_VECTOR_H
