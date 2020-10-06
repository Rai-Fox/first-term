â#include "big_integer.h"

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
	while (digits_.size() != size) {
		digits_.push_back(0);
	}
}

uint32_t get_digit(big_integer const& a, size_t ind, bool is_complement = false) {
	if (is_complement)
		return (ind < a.digits_.size() ? ~(a.digits_[ind]) + (ind == 0) : -1);
	else
		return (ind < a.digits_.size() ? a.digits_[ind] : 0);
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
	for (size_t i = (str[0] == '+' || str[0] == '-'); i < str.size(); i++) {
		if (!isdigit(str[i])) {
			throw std::runtime_error("Expected number, actual: " + str);
		}
		*this *= 10;
		*this += (str[i] - '0');
	}
	this->sign_ = (*this != 0 && str[0] == '-' ? _NEGATIVE : _POSITIVE);
}

big_integer operator+(big_integer a, big_integer const& b) {
	return a += b;
}

big_integer operator-(big_integer a, big_integer const& b) {
	return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b) {
	return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b) {
	return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b) {
	return a %= b;
}

big_integer big_integer::to_complement(size_t size) {
	resize_digits(size);
	if (sign_ == _NEGATIVE) {
		for (uint32_t& digit : digits_) {
			digit = ~digit;
		}
		*this -= 1;
	}
	return *this;
}

big_integer bit_operation(big_integer a, big_integer const& b, uint32_t(*op)(uint32_t, uint32_t)) {
	size_t max_size = std::max(a.digits_.size(), b.digits_.size());
	a.to_complement(max_size);
	for (size_t i = 0; i < max_size; i++) {
		a.digits_[i] = op(a.digits_[i], get_digit(b, i, b.sign_));
	}
	a.sign_ = op(a.sign_, b.sign_);
	a.to_complement(max_size);
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
	return a >>= shift;
}

big_integer operator<<(big_integer a, int shift) {
	return a <<= shift;
}

big_integer big_integer::operator~() const {
	return -*this - 1;
}

big_integer& big_integer::operator++() {
	*this += 1;
	return *this;
}

big_integer& big_integer::operator--() {
	*this -= 1;
	return *this;
}

big_integer big_integer::operator++(int) {
	big_integer x(*this);
	*this += 1;
	return x;
}

big_integer big_integer::operator--(int) {
	big_integer x(*this);
	*this -= 1;
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

big_integer& big_integer::operator+=(big_integer const& b) {
	if (b.digits_.empty()) {
		return *this;
	}
	if (sign_ == b.sign_) {
		size_t max_size = std::max(digits_.size(), b.digits_.size());
		resize_digits(max_size);
		uint64_t carry = 0;
		for (size_t i = 0; i < max_size; i++) {
			uint64_t tmp = static_cast<uint64_t>(digits_[i]) + static_cast<uint64_t>(get_digit(b, i)) + carry;
			carry = tmp > static_cast<uint64_t>(UINT32_MAX);
			digits_[i] = static_cast<uint32_t>(tmp);
		}
		if (carry)
			digits_.push_back(carry);
		normalize();
		return *this;
	} else {
		return *this -= (-b);
	}
}

big_integer& big_integer::operator-=(big_integer const& b) {
	if (b.digits_.empty()) {
		return *this;
	}
	if (sign_ == b.sign_) {
		big_integer result;
		bool a_sign = sign_;
		sign_ = b.sign_;
		bool less = (*this < b) ^ b.sign_;
		sign_ = a_sign ^ less;
		size_t max_size = std::max(digits_.size(), b.digits_.size());
		resize_digits(max_size);
		uint64_t carry = 0, tmp = 0;
		for (size_t i = 0; i < max_size; i++) {
			if (less) {
				tmp = static_cast<uint64_t>(UINT32_MAX) + 1ull - carry +
					static_cast<uint64_t>(get_digit(b, i)) - static_cast<uint64_t>(get_digit(*this, i));
			} else {
				tmp = static_cast<uint64_t>(UINT32_MAX) + 1ull - carry +
					static_cast<uint64_t>(get_digit(*this, i)) - static_cast<uint64_t>(get_digit(b, i));
			}
			carry = tmp <= static_cast<uint64_t>(UINT32_MAX);
			digits_[i] = static_cast<uint32_t>(tmp);
		}
		normalize();
		return *this;
	} else {
		return *this += (-b);
	}
}

big_integer& big_integer::operator*=(big_integer const& b) {
	big_integer result;
	result.resize_digits(digits_.size() + b.digits_.size() + 1);
	result.sign_ = (sign_ ^ b.sign_ ? _NEGATIVE : _POSITIVE);
	for (size_t i = 0; i < digits_.size(); i++) {
		uint32_t carry = 0;
		for (size_t j = 0; j < b.digits_.size(); j++) {
			uint64_t mul = static_cast<uint64_t>(digits_[i]) * b.digits_[j] + result.digits_[i + j] + carry;
			result.digits_[i + j] = static_cast<uint32_t>(mul);
			carry = static_cast<uint32_t>(mul >> static_cast<uint64_t>(32));
		}
		result.digits_[i + b.digits_.size()] += carry;
	}
	result.normalize();
	return *this = result;
}

uint32_t count_lz(uint32_t x) {
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

big_integer& big_integer::operator/=(big_integer const& _b) {
	big_integer ans;
	bool sign = sign_ ^ _b.sign_;
	big_integer b(abs(_b));
	sign_ = _POSITIVE;
	if (*this < b) {
		return *this = big_integer(0);
	}
	if (b.digits_.size() == 1) {
		ans = div_long_short(*this, b.digits_.back());
	} else {
		uint32_t shift = count_lz(b.digits_.back());
		*this <<= shift;
		b <<= shift;
		digits_.push_back(0);
		size_t n = digits_.size();
		size_t m = b.digits_.size() + 1;
		size_t k = n - m;
		ans.resize_digits(k + 1);
		for (size_t i = k + 1; i > 0; i--) {
			uint32_t qt = trial(*this, b);
			big_integer ml = b * qt;
			if (less(*this, ml, m)) {
				qt--;
				ml -= b;
			}
			ans.digits_[i - 1] = qt;
			difference(*this, ml, m);
			if (!digits_.back()) {
				digits_.pop_back();
			}
		}
		b >>= shift;
	}
	ans.sign_ = sign;
	ans.normalize();
	return *this = ans;
}

big_integer& big_integer::operator%=(big_integer const& b) {
	*this = *this - (*this / b) * b;
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
	*this /= (1u << (shift % 32u));
	size_t new_shift = std::min(static_cast<size_t>(shift / 32), digits_.size());
	for (size_t i = 0; i < digits_.size() - new_shift; i++) {
		digits_[i] = digits_[i + new_shift];
	}
	digits_.erase(digits_.begin() + digits_.size() - new_shift, digits_.begin() + digits_.size());
	return *this -= sign_ == _NEGATIVE;
}

big_integer& big_integer::operator<<=(int shift) {
	*this *= (1u << (shift % 32u));
	size_t new_shift = shift / 32;
	digits_.insert(digits_.begin(), new_shift, 0);
	return *this;
}

bool operator<(big_integer const& a, big_integer const& b) {
	if (a.sign_ != b.sign_)
		return b.sign_ == _POSITIVE;
	if (a.digits_.size() != b.digits_.size()) {
		return (a.sign_ == _NEGATIVE) ^ (a.digits_.size() < b.digits_.size());
	}
	for (size_t i = a.digits_.size(); i > 0; i--) {
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