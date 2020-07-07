#include "big_integer.h"

#define _POSITIVE (false)
#define _NEGATIVE (true)

void big_integer::normalize() {
	while (!digits_.empty() && digits_.back() == 0) {
		this->digits_.pop_back();
	}
	if (digits_.empty())
		sign_ = _POSITIVE;
}

void big_integer::resize_digits(size_t size) {
	normalize();
	while (digits_.size() != size) {
		digits_.push_back(0);
	}
}

big_integer abs(big_integer const& a) {
	big_integer result(a);
	result.sign_ = _POSITIVE;
	return result;
}

void swap(big_integer& a, big_integer& b) {
	big_integer tmp = a;
	a = b;
	b = tmp;
}

big_integer::big_integer() : digits_(), sign_(_POSITIVE) {}

big_integer::big_integer(big_integer const& a) {
	this->digits_ = a.digits_;
	this->sign_ = a.sign_;
}

big_integer::big_integer(int a) {
	if (a < 0) {
        this->sign_ = _NEGATIVE;
    }
	if (a != 0)
	    this->digits_.push_back(uint32_t(abs(a)));
}

big_integer::big_integer(std::string const& str) {
	for (const char &c : str) {
		if (c == '+' || c == '-') {
			continue;
		} else {
			*this *= 10;
			*this += (c - '0');
		}
	}
    this->sign_ = (str[0] == '-' ? _NEGATIVE : _POSITIVE);
}

big_integer& big_integer::operator=(big_integer const& other) {
	this->sign_ = other.sign_;
	this->digits_ = other.digits_;
	return *this;
}

big_integer operator+(big_integer const& _a, big_integer const& _b) {
	if (_a.sign_ == _b.sign_) {
		big_integer result;
		big_integer a(abs(_a));
		big_integer b(abs(_b));
		size_t max_size = std::max(a.digits_.size(), b.digits_.size());
		a.resize_digits(max_size);
		b.resize_digits(max_size);
		uint32_t carry = 0;
		result.sign_ = _a.sign_;
		for (size_t i = 0; i < max_size; i++) {
			if (a.digits_[i] == UINT32_MAX && carry == 1) {
				result.digits_.push_back(b.digits_[i]);
				carry = 1;
			} else {
				a.digits_[i] += carry;
				carry = (a.digits_[i] + b.digits_[i] < a.digits_[i]);
				result.digits_.push_back(a.digits_[i] + b.digits_[i]);
			}
		}
		if (carry != 0) {
			result.digits_.push_back(1);
		}
		result.normalize();
		return result;
	} else {
		return _a - (-_b);
	}
}

big_integer operator-(big_integer const& _a, big_integer const& _b) {
	if (_a.sign_ == _b.sign_) {
		big_integer result;
		big_integer a(abs(_a));
		big_integer b(abs(_b));
		size_t max_size = std::max(a.digits_.size(), b.digits_.size());
		a.resize_digits(max_size);
		b.resize_digits(max_size);
		uint32_t carry = 0;
		if (a < b) {
			result.sign_ = _b.sign_;
			swap(a, b);
		} else {
			result.sign_ = _a.sign_;
		}
		for (size_t i = 0; i < max_size; i++) {
			if (carry == 1 && a.digits_[i] == 0) {
				result.digits_.push_back(UINT32_MAX - b.digits_[i]);
				carry = 1;
			} else {
				a.digits_[i] -= carry;
				carry = (b.digits_[i] > a.digits_[i]);
				result.digits_.push_back(a.digits_[i] - b.digits_[i]);
			}
		}
		result.normalize();
		return result;
	} else {
		return _a + (-_b);
	}

}

big_integer operator*(big_integer const& _a, big_integer const& _b) {
	big_integer result;
	big_integer a(_a);
	big_integer b(_b);
	result.resize_digits(a.digits_.size() + b.digits_.size() + 1);
	result.sign_ = (a.sign_ ^ b.sign_ ? _NEGATIVE : _POSITIVE);
	for (size_t i = 0; i < a.digits_.size(); i++) {
		uint32_t carry = 0;
		for (size_t j = 0; j < b.digits_.size(); j++) {
			uint64_t mul = (uint64_t(a.digits_[i])) * b.digits_[j] + result.digits_[i + j] + carry;
			result.digits_[i + j] = mul;
			carry = uint32_t(mul >> uint64_t(32));
		}
		result.digits_[i + b.digits_.size()] += carry;
	}
	result.normalize();
	return result;
}

big_integer operator/(big_integer const& _a, big_integer const& _b) {
	return big_integer();
}	

big_integer big_integer::operator+() const {
	return big_integer(*this);
}

big_integer big_integer::operator-() const {
	big_integer result(*this);
	if (!this->digits_.empty())
	    result.sign_ ^= true;
	return result;
}

big_integer& big_integer::operator+=(big_integer const& other) {
	*this = *this + other;
	return *this;
}

big_integer& big_integer::operator-=(big_integer const& other) {
	*this = *this - other;
	return *this;
}

big_integer& big_integer::operator*=(big_integer const& other) {
	*this = *this * other;
	return *this;
}

bool operator<(big_integer const& _a, big_integer const& _b) {
	if (_a.sign_ != _b.sign_)
		return _b.sign_ == _POSITIVE;
	big_integer a(_a);
	big_integer b(_b);
	size_t max_size = std::max(a.digits_.size(), b.digits_.size());
	a.resize_digits(max_size);
	b.resize_digits(max_size);
	for (size_t i = max_size; i > 0; i--) {
		if (a.digits_[i - 1] != b.digits_[i - 1]) {
			return (a.sign_ == _NEGATIVE) ^ (a.digits_[i - 1] < b.digits_[i - 1]);
		}
	}
	return false;
}

bool operator>(big_integer const& a, big_integer const& b) {
	return b < a;
}

bool operator==(big_integer const& a, big_integer const& b) {
	return !(a < b) && !(a > b)
}

bool operator!=(big_integer const& a, big_integer const& b) {
	return !(a == b);
}

bool operator<=(big_integer const& a, big_integer const& b) {
	return !(a > b);
}

bool operator>=(big_integer const& a, big_integer const& b) {
	return !(a < b);
}

#undef _POSITIVE
#undef _NEGATIVE