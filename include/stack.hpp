#include <iostream>
#include <stdexcept>


template<typename T>
class allocator
{
protected:
	allocator(size_t size = 0);
	~allocator();
	auto swap(allocator& other)->void;
	allocator(allocator const&) = delete;
	auto operator=(allocator const&)->allocator& = delete;
	T * _array;
	size_t _size;
	size_t _count;
};




template <typename T1, typename T2>
auto construct(T1 * ptr, T2 const & value)->void {
	new(ptr) T1(value);
}

template <typename T>
void destroy(T * ptr) noexcept
{
	ptr->~T();
}

template <typename FwdIter>
void destroy(FwdIter first, FwdIter last) noexcept
{
	for (; first != last; ++first) {
		destroy(&*first);
	}
}

template <typename T>
allocator<T>::allocator(size_t size) : _array(static_cast<T *>(size == 0 ? nullptr : operator new(size * sizeof(T)))), _size(0), _count(size) {
};

template <typename T>
allocator<T>::~allocator() {
operator delete(_array);
};

template <typename T>
auto allocator<T>::swap(allocator& other)->void {
	std::swap(_array, other._array);
	std::swap(_size, other._size);
	std::swap(_count, other._count);
};


template <typename T>
class stack : private allocator<T>
{
public:
	stack();/*noexcept*/
	stack(stack const &); /*strong*/
	auto count() const noexcept->size_t;/*noexcept*/
	auto push(T const &)->void;/*strong*/
	auto pop()->void;/*strong*/
	auto top() const->T&;/*strong*/
	~stack(); 	/*noexcept*/
	auto operator=(const stack&tmp)->stack&;/*strong*/
	auto empty()->bool;/*noexcept*/

};




template<typename T> // ïðîâåðêà íà ïóñòîòó ñòýêà 
auto stack<T>::empty()->bool {
	return (allocator<T>::_count == 0);
}

template <typename T>//êîïèðîâàíèå è âûäåëåíèå ïàìÿòè 
auto mem_copy(size_t count_m, size_t array_size_m, const T * tmp)->T* {
	T *mass = new T[array_size_m];
	std::copy(tmp, tmp + count_m, mass);
	return mass;
}


template <typename T>
stack<T>::~stack() {
destroy(allocator<T>::_array, allocator<T>::_array + allocator<T>::_count);};

template <typename T>
stack<T>::stack()  {};



template <typename T>
auto stack<T>::push(T const &val)->void {
	if (allocator<T>::_count == allocator<T>::_size) {
		size_t size = allocator<T>::_size * 2 + (allocator<T>::_size == 0);
		T *tmp = mem_copy(allocator<T>::_count, size, allocator<T>::_array);
		delete[] allocator<T>::_array;
		allocator<T>::_array = tmp;
		allocator<T>::_size = size;
	}
	construct(allocator<T>::_array +allocator<T>::_count,val);
	++allocator<T>::_count;
}


template <typename T>
stack<T>::stack(stack const &tmp) {
	allocator<T>::_count = tmp._count;
	allocator<T>::_array = mem_copy(tmp._count, tmp._size, tmp._array);
	allocator<T>::_size = tmp._size;
};

template <typename T>
auto stack<T>::operator=(const stack &tmp)->stack& {
	if (this != &tmp) {
		T* cp = mem_copy(tmp._count, tmp._size, tmp._array);
		delete[] allocator<T>::_array;
		allocator<T>::_array = cp;
		allocator<T>::_size =tmp._size;
		allocator<T>::_count = tmp._count;
	}
	return *this;
}


template <typename T>
auto stack<T>::count() const noexcept->size_t {
	return allocator<T>::_count;
}

template <typename T>
auto stack<T>::pop()->void {
	if (allocator<T>::_count == 0) throw std::logic_error("Empty!");
	destroy(allocator<T>::_array +allocator<T>::_count);
	--allocator<T>::_count;
}

template <typename T>
auto stack<T>::top() const->T& {
	if (allocator<T>::_count == 0) throw std::logic_error("Empty!");
	return allocator<T>::_array[allocator<T>::_count - 1];

}
