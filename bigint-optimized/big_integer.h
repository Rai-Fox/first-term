#pragma once
#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include "optimized_vector.h"

struct big_integer {
private:
	using storage_t = optimized_vector;
	storage_t digits_;
	bool sign_ = false;

	big_integer(uint32_t a);
	void resize_digits(size_t size);
	void normalize();
	friend big_integer abs(big_integer const&);
	friend void swap(big_integer&, big_integer&);
	friend big_integer div_long_short(big_integer const&, uint32_t);
	friend uint32_t trial(big_integer const&, big_integer const&);
	friend uint32_t get_digit(big_integer const&, size_t, bool);
	friend bool less(big_integer const&, big_integer const&, size_t);
	friend void difference(big_integer&, big_integer const&, size_t);
	friend big_integer bit_operation(big_integer, big_integer const&, uint32_t(*op)(uint32_t, uint32_t));
	friend uint32_t count_lz(uint32_t);
	big_integer to_complement(size_t size);
	void sum(big_integer const&);
	void subtract(big_integer const&);
	void additive_operation(big_integer const&, bool);
public:
	big_integer() = default;
	big_integer(big_integer const& a) = default;
	big_integer(int a);
	explicit big_integer(std::string const& str);

	big_integer& operator=(big_integer const& other) = default;

	friend big_integer operator+(big_integer a, big_integer const& b);
	friend big_integer operator-(big_integer a, big_integer const& b);
	friend big_integer operator*(big_integer a, big_integer const& b);
	friend big_integer operator/(big_integer a, big_integer const& b);
	friend big_integer operator%(big_integer a, big_integer const& b);

	friend big_integer operator&(big_integer const& a, big_integer const& b);
	friend big_integer operator|(big_integer const& a, big_integer const& b);
	friend big_integer operator^(big_integer const& a, big_integer const& b);

	friend big_integer operator<<(big_integer a, int b);
	friend big_integer operator>>(big_integer a, int b);

	big_integer operator+() const;
	big_integer operator-() const;
	big_integer& operator++();
	big_integer& operator--();
	big_integer operator++(int);
	big_integer operator--(int);
	big_integer operator~() const;

	big_integer& operator+=(big_integer const&);
	big_integer& operator-=(big_integer const&);
	big_integer& operator*=(big_integer const&);
	big_integer& operator/=(big_integer const&);
	big_integer& operator%=(big_integer const&);

	big_integer& operator&=(big_integer const&);
	big_integer& operator|=(big_integer const&);
	big_integer& operator^=(big_integer const&);

	big_integer& operator<<=(int rhs);
	big_integer& operator>>=(int rhs);

	friend bool operator==(big_integer const&, big_integer const&);
	friend bool operator!=(big_integer const&, big_integer const&);
	friend bool operator<(big_integer const&, big_integer const&);
	friend bool operator>(big_integer const&, big_integer const&);
	friend bool operator<=(big_integer const&, big_integer const&);
	friend bool operator>=(big_integer const&, big_integer const&);

	friend std::string to_string(big_integer a);
};
#endif // BIG_INTEGER_H