#pragma once

#include<memory>
#include<cassert>
#include<algorithm>
#include<iterator>

template< class T, class Alloc = std::allocator<T> > class myvector
{
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type const_pointer;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;
protected:
	std::allocator<value_type> _alloc;
	iterator _start;
	iterator _end;
	iterator _end_of_storage;

public:
	myvector() : _start(0), _end(0), _end_of_storage(0) {}
	myvector(size_type n, const T& value);
	myvector(size_type n);
	myvector(iterator first, iterator last);
	myvector(const myvector& v);
	myvector& operator=(const myvector& rhs);
	~myvector()
	{
		_destroy(_start, _end);
		_alloc.deallocate(_start, _end_of_storage - _start);
		_start = _end = _end_of_storage = nullptr;
	}

	iterator begin() const { return _start; }
	iterator end() const { return _end; }
	const_iterator cbegin() const { return _start; }
	const_iterator cend() const { return _end; }
	std::reverse_iterator<iterator> rbegin() const { return std::reverse_iterator<iterator>(end()); }
	std::reverse_iterator<iterator> rend() const { return std::reverse_iterator<iterator>(begin()); }
	std::reverse_iterator<const_iterator> crbegin() const { return std::reverse_iterator<const_iterator>(cend()); }
	std::reverse_iterator<const_iterator> crend() const { return std::reverse_iterator<const_iterator>(cbegin()); }

	size_type size() const { return static_cast<size_type>(_end - _start); }
	size_type capacity() const { return static_cast<size_type>(_end_of_storage - _start); }
	bool empty() const { return begin() == end(); }
	void swap(myvector& v);

	reference front() const { return *begin(); }
	reference back() const { return *(end() - 1); }
	reference operator[] (size_type n) {
		assert(n < size());
		return *(begin() + n);
	}

	void push_back(const T& value);
	void pop_back();
	void insert(iterator position, size_type n, const T& value);

	iterator erase(iterator position);
	iterator erase(iterator first, iterator last);
	void clear() { erase(begin(), end()); }

private:
	void _destroy(iterator first, iterator last);
	void insert_aux(iterator position, const T& value);
};

template <class T, class Alloc>
myvector<T, Alloc>::myvector(size_type n, const T& value)
{
	_start = _alloc.allocate(n);
	std::uninitialized_fill(_start, _start + n, value);
	_end = _end_of_storage = _start + n;
}

template <class T, class Alloc>
myvector<T, Alloc>::myvector(size_type n)
{
	_start = _alloc.allocate(n);
	std::uninitialized_fill(_start, _start + n, 0);
	_end = _end_of_storage = _start + n;
}

template <class T, class Alloc>
myvector<T, Alloc>::myvector(iterator first, iterator last)
{
	_start = _alloc.allocate(last - first);
	_end = _end_of_storage = std::uninitialized_copy(first, last, first);
}

template <class T, class Alloc>
myvector<T, Alloc>::myvector(const myvector& v)
{
	_start = _alloc.allocate(v.size());
	_end = _end_of_storage = std::uninitialized_copy(v.cbegin(), v.cend(), _start);
}

template <class T, class Alloc>
myvector<T, Alloc>& myvector<T, Alloc>::operator=(const myvector& v)
{
	myvector<T, Alloc> temp(v);
	swap(temp);
	return *this;
}

template <class T, class Alloc>
void myvector<T, Alloc>::swap(myvector& v)
{
	using std::swap;
	swap(_start, v._start);
	swap(_end, v._end);
	swap(_end_of_storage, v._end_of_storage);
}

template <class T, class Alloc>
void myvector<T, Alloc>::_destroy(iterator first, iterator last)
{
	if (first)
	{
		iterator it(last);
		while (it != first)
			_alloc.destroy(--it);
	}
}

template <class T, class Alloc>
void myvector<T, Alloc>::insert(iterator position, size_type n, const T& value)
{
	if (n <= 0)
		return;
	if (_end_of_storage - _end >= static_cast<int>(n))
	{
		const size_type element_after = _end - position;
		if (element_after > n)
		{
			std::uninitialized_copy(_end - n, _end, _end);
			std::copy_backward(position, _end - n, _end);
			std::fill(position, position + n, value);
			_end += n;
		}
		else {
			iterator old_end = _end;
			_end += n;
			std::copy_backward(position, old_end, _end);
			std::fill(position, position + n, value);
		}
	}
	else {
		const size_type old_size = size();
		const size_type new_size = old_size + std::max(old_size, n);

		iterator new_start = _alloc.allocate(new_size);
		iterator new_end = new_start;

		new_end = std::uninitialized_copy(_start, position, new_start);
		std::uninitialized_fill_n(new_end, n, value);
		new_end += n;
		new_end = std::uninitialized_copy(position, _end, new_end);

		_destroy(_start, _end);
		_alloc.deallocate(_start, _end_of_storage - _start);

		_start = new_start;
		_end = new_end;
		_end_of_storage = _start + new_size;
	}
}

template <class T, class Alloc>
void myvector<T, Alloc>::pop_back()
{
	--_end;
	_alloc.destroy(_end);
}

template <class T, class Alloc>
void myvector<T, Alloc>::push_back(const T& value)
{
	if (_end != _end_of_storage)
	{
		_alloc.construct(_end, value);
		++_end;
	}
	else {
		insert_aux(end(), value);
	}
}

template <class T, class Alloc>
void myvector<T, Alloc>::insert_aux(iterator position, const T& value)
{
	const size_type old_size = size();
	const size_type new_size = old_size ? 2 * old_size : 1;
	iterator new_start = _alloc.allocate(new_size);
	iterator new_end = new_start;
	new_end = std::uninitialized_copy(_start, _end, new_start);
	_alloc.construct(new_end, value);
	++new_end;

	_destroy(_start, _end);
	_alloc.deallocate(_start, _end_of_storage - _start);

	_start = new_start;
	_end = new_end;
	_end_of_storage = _start + new_size;
}

template <class T, class Alloc>
typename myvector<T, Alloc>::iterator myvector<T, Alloc>::erase(iterator first, iterator last)
{
	iterator it = std::copy(last, _end, first);
	_destroy(it, _end);
	_end -= last - first;
	return first;
}

template <class T, class Alloc>
typename myvector<T, Alloc>::iterator myvector<T, Alloc>::erase(iterator position)
{
	return erase(position, position + 1);
}