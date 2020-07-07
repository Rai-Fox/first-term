#pragma once
#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

struct big_integer {
private:
	std::vector<uint32_t> digits_;
	bool sign_ = false;
	void resize_digits(size_t size);
	void normalize();
	friend big_integer abs(big_integer const&);
	friend void swap(big_integer &, big_integer &);
public:
	big_integer();
	big_integer(big_integer const& a);
	big_integer(int a);
	explicit big_integer(std::string const& str);

	big_integer& operator=(big_integer const& other);

	friend big_integer operator+(big_integer const& a, big_integer const& b);
	friend big_integer operator-(big_integer const& a, big_integer const& b);
	friend big_integer operator*(big_integer const& a, big_integer const& b);
	//friend big_integer operator/(big_integer const& a, big_integer const& b);
	//friend big_integer operator%(big_integer const& a, big_integer const& b);

	//friend big_integer operator&(big_integer const& a, big_integer const& b);
	//friend big_integer operator|(big_integer const& a, big_integer const& b);
	//friend big_integer operator^(big_integer const& a, big_integer const& b);

	big_integer operator+() const;
	big_integer operator-() const;
	//big_integer operator~() const;

	big_integer& operator+=(big_integer const&);
	big_integer& operator-=(big_integer const&);
	big_integer& operator*=(big_integer const&);
	/*big_integer& operator/=(big_integer const&);
	big_integer& operator%=(big_integer const&);

	big_integer& operator&=(big_integer const&);
	big_integer& operator|=(big_integer const&);
	big_integer& operator^=(big_integer const&);*/

	friend bool operator==(big_integer const&, big_integer const&);		
	friend bool operator!=(big_integer const&, big_integer const&);
	friend bool operator<(big_integer const&, big_integer const&);
	friend bool operator>(big_integer const&, big_integer const&);
	friend bool operator<=(big_integer const&, big_integer const&);
	friend bool operator>=(big_integer const&, big_integer const&);

	//friend std::string to_string(big_integer const& a);
};	
#endif // BIG_INTEGER_H