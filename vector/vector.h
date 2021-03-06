#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm>
template <typename T>
struct vector
{
    typedef T* iterator;
    typedef T const* const_iterator;

    vector();                               // O(1) nothrow
    vector(vector const&);                  // O(N) strong
    vector& operator=(vector const& other); // O(N) strong

    ~vector();                              // O(N) nothrow

    T& operator[](size_t i);                // O(1) nothrow
    T const& operator[](size_t i) const;    // O(1) nothrow

    T* data();                              // O(1) nothrow
    T const* data() const;                  // O(1) nothrow
    size_t size() const;                    // O(1) nothrow

    T& front();                             // O(1) nothrow
    T const& front() const;                 // O(1) nothrow

    T& back();                              // O(1) nothrow
    T const& back() const;                  // O(1) nothrow
    void push_back(T const&);               // O(1)* strong
    void pop_back();                        // O(1) nothrow

    bool empty() const;                     // O(1) nothrow

    size_t capacity() const;                // O(1) nothrow
    void reserve(size_t);                   // O(N) strong
    void shrink_to_fit();                   // O(N) strong

    void clear();                           // O(N) nothrow

    void swap(vector&);                     // O(1) nothrow

    iterator begin();                       // O(1) nothrow
    iterator end();                         // O(1) nothrow

    const_iterator begin() const;           // O(1) nothrow
    const_iterator end() const;             // O(1) nothrow

    iterator insert(iterator pos, T const&); // O(N) weak
    iterator insert(const_iterator pos, T const&); // O(N) weak

    iterator erase(iterator pos);           // O(N) weak
    iterator erase(const_iterator pos);     // O(N) weak

    iterator erase(iterator first, iterator last); // O(N) weak
    iterator erase(const_iterator first, const_iterator last); // O(N) weak

private:
	vector(T const*, size_t size, size_t capacity);
private:
    T* data_;
    size_t size_;
    size_t capacity_;
};

template<typename T> 
vector<T>::vector() :
data_(nullptr), size_(0), capacity_(0) {}

template<typename T>
vector<T>::vector(T const* data, size_t size, size_t capacity) {
    capacity_ = capacity;
	data_ = capacity ? static_cast<T*>(operator new(capacity * sizeof(T))) : nullptr;
	try {
        for (size_ = 0; size_ < size; size_++)
            new(end()) T(data[size_]);
    } catch (...) {
	    clear();
	    operator delete(data_);
	    throw;
	}
}

template<typename T>
vector<T>::vector(vector const& other) :
vector(other.data_, other.size_, other.size_) {}

template<typename T>
vector<T>::~vector() {
    clear();
    operator delete(data_);
}

template<typename T>
vector<T>& vector<T>::operator=(vector const& other) {
    vector<T> new_vector(other);
    swap(new_vector);
    return *this;
}

template<typename T>
T& vector<T>::operator[](size_t i) {
	return data_[i];
}

template<typename T>
T const& vector<T>::operator[](size_t i) const {
	return data_[i];
}

template<typename T>
T* vector<T>::data() {
    return data_;
}

template<typename T>
T const* vector<T>::data() const {
    return data_;
}

template<typename T>
typename vector<T>::iterator vector<T>::begin() {
    return data_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::begin() const {
    return data_;
}

template<typename T>
typename vector<T>::iterator vector<T>::end() {
    return (data_ + size_);
}

template<typename T>
typename vector<T>::const_iterator vector<T>::end() const {
    return (data_ + size_);
}

template<typename T>
T& vector<T>::front() {
    return *begin();
}

template<typename T>
T const& vector<T>::front() const {
    return *begin();
}

template<typename T>
T& vector<T>::back() {
    return *(end() - 1);
}

template<typename T>
T const& vector<T>::back() const {
    return *(end() - 1);
}


template<typename T>
void vector<T>::swap(vector& other) {
	using std::swap;
    swap(other.data_, data_);
    swap(other.size_, size_);
    swap(other.capacity_, capacity_);
}

template<typename T>
size_t vector<T>::size() const {
    return size_;
}

template<typename T>
size_t vector<T>::capacity() const {
    return capacity_;
}

template<typename T>
void vector<T>::pop_back() {
	size_--;
	data_[size_].~T();
}

template<typename T>
void vector<T>::clear() {
    while (size_)
        pop_back();
}

template<typename T>
bool vector<T>::empty() const {
    return (size_ == 0);
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (capacity_ == size_)
        return;
    vector<T> new_vector(data_, size_, size_);
    swap(new_vector);
}

template<typename T>
void vector<T>::push_back(T const& el) {
	if (size_ == capacity_) {
		T copy_el = T(el);
		reserve(capacity_ * 2 + 1);
		new (end()) T(copy_el);
	} else {
		new (end()) T(el);
	}
	size_++;
}

template<typename T>
void vector<T>::reserve(size_t new_capacity) {
    if (capacity_ >= new_capacity)
        return;
    vector<T> new_vector(data_, size_, new_capacity);
    swap(new_vector);
}

template<typename T>
typename vector<T>::iterator vector<T>::insert(iterator pos, T const& el) {
	using std::swap;
	ptrdiff_t ind = pos - data_;
	push_back(el);
	for (size_t i = size_ - 1; i > ind; i--)
		swap(data_[i], data_[i - 1]);
	return begin() + ind;
}

template<typename T>
typename vector<T>::iterator vector<T>::insert(const_iterator pos, T const& el) {
	using std::swap;
	ptrdiff_t ind = pos - data_;
	push_back(el);
	for (size_t i = size_ - 1; i > ind; i--)
		swap(data_[i], data_[i - 1]);
	return begin() + ind;
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(iterator pos) {
	return erase(pos, pos + 1);
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
	return erase(pos, pos + 1);
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(iterator first, iterator last) {
	using std::swap;
	ptrdiff_t ind = first - data_;
	ptrdiff_t len = last - first;
	for (; ind != size_ - len; ind++)
		swap(data_[ind], data_[ind + len]);
	while (len--) {
		pop_back();
	}
	return begin() + ind;
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last) {
	using std::swap;
	ptrdiff_t ind = first - data_;
	ptrdiff_t len = last - first;
	for (; ind != size_ - len; ind++)
		swap(data_[ind], data_[ind + len]);
	while (len--) {
		pop_back();
	}
	return begin() + ind;
}
#endif