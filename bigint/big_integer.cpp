#include "big_integer.h"

using uint128_t = unsigned __int128;

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
	using std::swap;
	swap(a.digits_, b.digits_);
	swap(a.sign_, b.sign_);
}

big_integer::big_integer() : digits_(), sign_(_POSITIVE) {}

big_integer::big_integer(big_integer const& a) {
	this->digits_ = a.digits_;
	this->sign_ = a.sign_;
}

big_integer::big_integer(int a) {
	this->sign_ = a < 0;
	if (a != 0) {
		this->digits_.push_back(static_cast<uint32_t>(std::abs(static_cast<int64_t>(a))));
	}
}

big_integer::big_integer(uint32_t a) {
	this->sign_ = _POSITIVE;
	if (a != 0) {
		this->digits_.push_back(a);
	}
}

big_integer::big_integer(std::string const& str) {
	for (const char& c : str) {
		if (c == '+' || c == '-') {
			continue;
		} else {
			*this *= 10;
			*this += (c - '0');
		}
	}
	this->sign_ = (*this != 0 && str[0] == '-' ? _NEGATIVE : _POSITIVE);
}

big_integer& big_integer::operator=(big_integer const& other) {
	this->sign_ = other.sign_;
	this->digits_ = other.digits_;
	return *this;
}

big_integer operator+(big_integer const& _a, big_integer const& _b) {
	if (_b.digits_.empty()) {
		return _a;
	}
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
	if (_b.digits_.empty()) {
		return _a;
	}
	if (_a.sign_ == _b.sign_) {
		big_integer result;
		big_integer a(abs(_a));
		big_integer b(abs(_b));
		if (a < b) {
			result.sign_ = !_a.sign_;
			swap(a, b);
		} else {
			result.sign_ = _a.sign_;
		}
		size_t max_size = std::max(a.digits_.size(), b.digits_.size());
		a.resize_digits(max_size);
		b.resize_digits(max_size);
		uint32_t carry = 0;
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
			uint64_t mul = static_cast<uint64_t>(a.digits_[i]) * b.digits_[j] + result.digits_[i + j] + carry;
			result.digits_[i + j] = static_cast<uint32_t>(mul);
			carry = static_cast<uint32_t>(mul >> static_cast<uint64_t>(32));
		}
		result.digits_[i + b.digits_.size()] += carry;
	}
	result.normalize();
	return result;
}

uint32_t count_lz(uint32_t x) {
	uint32_t ans = 0;
	for (uint32_t i = 31; i > 0; i--) {
		if (x & (1u << i))
			return 31u - i;
	}
	return 31;
}

big_integer div_long_short(big_integer const& a, uint32_t b) {
	big_integer result;
	uint64_t tmp = 0;
	for (size_t i = a.digits_.size(); i > 0; i--) {
		tmp = (tmp << static_cast<uint64_t>(32)) + a.digits_[i - 1];
		result.digits_.push_back(tmp / static_cast<uint64_t>(b));
		tmp = tmp % b;
	}
	std::reverse(result.digits_.begin(), result.digits_.end());
	result.normalize();
	return result;
}

uint32_t trial(big_integer const& a, big_integer const& b) {
	uint128_t x = (static_cast<uint128_t>(a.digits_[a.digits_.size() - 1]) << 64u) |
		(static_cast<uint128_t>(a.digits_[a.digits_.size() - 2]) << 32u) |
		(static_cast<uint128_t>(a.digits_[a.digits_.size() - 3]));
	uint128_t y = ((static_cast<uint128_t>(b.digits_[b.digits_.size() - 1]) << 32u) |
		(static_cast<uint128_t>(b.digits_[b.digits_.size() - 2])));
	return static_cast<uint32_t>(std::min(static_cast<uint128_t>(UINT32_MAX), x / y));
}

uint32_t get_digit(big_integer const& a, size_t ind) {
	return (ind < a.digits_.size() ? a.digits_[ind] : 0);
}

void difference(big_integer& a, big_integer const& b, size_t ind) {
	size_t start = a.digits_.size() - ind;
	uint32_t c = 0;
	for (size_t i = 0; i < ind; i++) {
		uint32_t cur = a.digits_[start + i];
		a.digits_[start + i] = static_cast<uint32_t>(static_cast<uint64_t>(cur) - get_digit(b, i) - c);
		c = get_digit(b, i) + c > cur;
	}
}

bool less(big_integer const& a, big_integer const& b, size_t ind) {
	for (size_t i = 1; i <= a.digits_.size(); i++) {
		if (a.digits_[a.digits_.size() - i] != get_digit(b, ind - i)) {
			return a.digits_[a.digits_.size() - i] < get_digit(b, ind - i);
		}
	}
	return false;
}

big_integer operator/(big_integer const& _a, big_integer const& _b) {
	big_integer a(abs(_a));
	big_integer b(abs(_b));
	big_integer ans;
	if (a < b) {
		return big_integer(0);
	}
	if (b.digits_.size() == 1) {
		ans = div_long_short(a, b.digits_.back());
	} else {
		uint32_t shift = count_lz(b.digits_.back());
		a <<= shift;
		b <<= shift;
		a.digits_.push_back(0);
		size_t n = a.digits_.size();
		size_t m = b.digits_.size() + 1;
		size_t k = n - m;
		ans.resize_digits(k + 1);
		for (size_t i = k + 1; i > 0; i--) {
			uint32_t qt = trial(a, b);
			big_integer ml = b * qt;
			if (less(a, ml, m)) {
				qt--;
				ml -= b;
			}
			ans.digits_[i - 1] = qt;
			difference(a, ml, m);
			if (!a.digits_.back()) {
				a.digits_.pop_back();
			}
		}
	}
	ans.sign_ = _a.sign_ ^ _b.sign_;
	ans.normalize();
	return ans;
}


big_integer operator%(big_integer const& a, big_integer const& b) {
	return a - (a / b) * b;
}

big_integer to_complement(big_integer a, size_t size) {
	a.resize_digits(size);
	if (a.sign_ == _NEGATIVE) {
		for (uint32_t& digit : a.digits_) {
			digit = ~digit;
		}
		a -= 1;
	}
	return a;
}

big_integer bit_operation(big_integer a, big_integer b, uint32_t(*op)(uint32_t, uint32_t)) {
	size_t max_size = std::max(a.digits_.size(), b.digits_.size());
	a = to_complement(a, max_size);
	b = to_complement(b, max_size);
	for (size_t i = 0; i < max_size; i++) {
		a.digits_[i] = op(a.digits_[i], b.digits_[i]);
	}
	a.sign_ = op(a.sign_, b.sign_);
	a = to_complement(a, max_size);
	a.normalize();
	return a;
}

big_integer operator&(big_integer const& a, big_integer const& b) {
	return bit_operation(a, b, [](uint32_t x, uint32_t y) {return x & y; });
}

big_integer operator|(big_integer const& a, big_integer const& b) {
	return bit_operation(a, b, [](uint32_t x, uint32_t y) {return x | y; });
}

big_integer operator^(big_integer const& a, big_integer const& b) {
	return bit_operation(a, b, [](uint32_t x, uint32_t y) {return x ^ y; });
}

big_integer operator>>(big_integer a, int shift) {
	a /= (1u << (shift % 32u));
	size_t new_shift = std::min(static_cast<size_t>(shift / 32), a.digits_.size());
	for (size_t i = 0; i < a.digits_.size() - new_shift; i++) {
		a.digits_[i] = a.digits_[i + new_shift];
	}
	a.digits_.erase(a.digits_.begin() + a.digits_.size() - new_shift, a.digits_.begin() + a.digits_.size());
	a -= a.sign_ == _NEGATIVE;
	return a;
}

big_integer operator<<(big_integer a, int shift) {
	a *= (1u << (shift % 32u));
	size_t new_shift = shift / 32;
	a.digits_.insert(a.digits_.begin(), new_shift, 0);
	return a;
}

big_integer big_integer::operator~() const {
	return -*this - 1;
}

big_integer& big_integer::operator++() {
	*this = *this + 1;
	return *this;
}

big_integer& big_integer::operator--() {
	*this = *this - 1;
	return *this;
}

big_integer big_integer::operator++(int) {
	big_integer x(*this);
	*this = *this + 1;
	return x;
}

big_integer big_integer::operator--(int) {
	big_integer x(*this);
	*this = *this - 1;
	return x;
}

big_integer big_integer::operator+() const {
	return big_integer(*this);
}

big_integer big_integer::operator-() const {
	big_integer result(*this);
	if (!result.digits_.empty())
		result.sign_ = !result.sign_;
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

big_integer& big_integer::operator/=(big_integer const& other) {
	*this = *this / other;
	return *this;
}

big_integer& big_integer::operator%=(big_integer const& other) {
	*this = *this % other;
	return *this;
}

big_integer& big_integer::operator&=(big_integer const& other) {
	*this = *this & other;
	return *this;
}

big_integer& big_integer::operator|=(big_integer const& other) {
	*this = *this | other;
	return *this;
}

big_integer& big_integer::operator^=(big_integer const& other) {
	*this = *this ^ other;
	return *this;
}

big_integer& big_integer::operator>>=(int shift) {
	*this = *this >> shift;
	return *this;
}

big_integer& big_integer::operator<<=(int shift) {
	*this = *this << shift;
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
	return !(a < b) && !(a > b);
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

std::string to_string(big_integer x) {
	bool sign = x.sign_;
	x = abs(x);
	std::string result;
	while (x != 0) {
		big_integer rem = x % 10;
		result.push_back('0' + (rem.digits_.empty() ? 0 : rem.digits_[0]));
		x /= 10;
	}
	if (sign) {
		result.push_back('-');
	}
	std::reverse(result.begin(), result.end());
	return (result.empty() ? "0" : result);
}

#undef _POSITIVE
#undef _NEGATIVE