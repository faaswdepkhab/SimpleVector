#pragma once
 
#include <algorithm>
#include "array_ptr.h"
#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <iterator>
 
struct ReserveProxyObj {
    size_t capacity_to_reserve_;
    ReserveProxyObj(size_t capacity_to_reserve)
        : capacity_to_reserve_(capacity_to_reserve) {}
};
 
ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}
 
template <typename Type>
class SimpleVector {
private:
    ArrayPtr<Type> raw_vector_;
    size_t size_ = 0;
    size_t capacity_ = 0;
 
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;
 
    SimpleVector() noexcept = default;
 
    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        ArrayPtr<Type> array_copy(size);
        array_copy.swap(raw_vector_);
        size_ = size;
        capacity_ = size;
    }
	
    // конструктор с резервированием
    SimpleVector(ReserveProxyObj obj) {
    	Reserve(obj.capacity_to_reserve_);
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, Type value) {
        ArrayPtr<Type> array_copy(size);
    	std::fill(array_copy.Get(), array_copy.Get() + size, value);
    	array_copy.swap(raw_vector_);
    	size_ = size;
    	capacity_ = size;
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
	    ArrayPtr<Type> array_copy(init.size());
	    std::copy(std::make_move_iterator(init.begin()), std::make_move_iterator(init.end()), array_copy.Get());
	    array_copy.swap(raw_vector_);
	    size_ = init.size();
	    capacity_ = init.size();
    }
	
    // Меняет вектора местами
    void swap(SimpleVector<Type> &other) noexcept {
    	raw_vector_.swap(other.raw_vector_);
    	std::swap(size_, other.size_);
    	std::swap(capacity_, other.capacity_);
    }
    
    // конструктор копирования
    SimpleVector(const SimpleVector &other) {
    	SimpleVector<Type> temp(other.size_);
    	std::copy(other.begin(), other.end(), temp.begin());
    	temp.size_ = other.size_;
    	temp.capacity_ = other.capacity_;
    	swap(temp);
    }
    
    // конструктор перемещения
    SimpleVector(SimpleVector &&other) {
    	raw_vector_ = std::move(other.raw_vector_);
    	size_ = std::move(other.size_);
    	capacity_ = size_;
    	other.size_ = 0;
    	other.capacity_ = 0;
    }

    // оператор присваивания
    SimpleVector& operator=(SimpleVector rhs) {
    	if (raw_vector_.Get() != rhs.raw_vector_.Get()) {
            ArrayPtr<Type> array_copy(rhs.size_);
            std::copy(std::make_move_iterator(rhs.raw_vector_.Get()), std::make_move_iterator(rhs.raw_vector_.Get() + rhs.size_), array_copy.Get());
            raw_vector_.swap(array_copy);
            size_ = std::move(rhs.size_); 
            capacity_ = std::move(rhs.size_);
        }
    	return *this;
    }
    
    
    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
    	return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
    	return capacity_;
    }
    
    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
    	return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
    	assert(index < size_);
    	return raw_vector_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
    	assert(index < size_);
    	return raw_vector_[index];
    }
    
    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
    	if (index >= size_) {
        	throw std::out_of_range("Index is out of range");
    	}
    	return raw_vector_[index];
    }
    
    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
    	if (index >= size_) {
        	throw std::out_of_range("Index is out of range");
    	}
    	return raw_vector_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
    	size_ = 0;
    }
    
    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
    	if (new_size <= capacity_) {
        	if (new_size <= size_) {
        	    size_ = new_size;
        	} else {
        	    std::fill(raw_vector_.Get() + size_, raw_vector_.Get() + new_size, Type{});
        	    size_ = new_size;
        	}
    	} else {
    	    SimpleVector<Type> temp(std::max(capacity_ * 2, new_size));
    	    std::copy(begin(), end(), temp.begin());
    	    swap(temp);
    	}
    }
    
    // изменяет кол-во зарезервированного места
    void Reserve(size_t new_capacity) {
    	if (new_capacity > capacity_) {
    	    SimpleVector<Type> temp(new_capacity);
    	    std::copy(begin(), end(), temp.begin());
    	    temp.size_ = size_;
    	    swap(temp);
    	}
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
    	return raw_vector_.Get();
    }
    
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
    	return raw_vector_.Get() + size_;
    }
    
    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
    	return raw_vector_.Get();
    }
    
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
    	return raw_vector_.Get() + size_;
    }
    
    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
    	return raw_vector_.Get();
    }
    
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
    	return raw_vector_.Get() + size_;
    }
    
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type item) {
        if (size_ < capacity_) {
            raw_vector_[size_] = std::move(item);
            ++size_;
        } else {
            SimpleVector<Type> temp(std::max(size_ * 2, static_cast<size_t>(1)));
            temp.size_ = size_ + 1;
    	    std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), temp.begin());
    	    temp[size_] = std::move(item);
    	    swap(temp);
    	}
    }
    
    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
    	assert(!IsEmpty());
    	--size_;
    }
    
    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, Type value) {
        assert((pos >= begin()) && (pos <= end()));
    	size_t index = static_cast<size_t>(pos - begin());
    	if (size_ < capacity_) {
        	std::copy_backward(std::make_move_iterator(begin() + index), std::make_move_iterator(end()), end() + 1);
        	raw_vector_[index] = std::move(value);
        	++size_;
    	} else {
        	SimpleVector<Type> temp(std::max(size_ * 2, static_cast<size_t>(1)));
        	temp.size_ = size_ + 1;
        	std::copy(std::make_move_iterator(begin()), std::make_move_iterator(begin() + index), temp.begin());
        	temp[index] = std::move(value);
        	std::copy(std::make_move_iterator(begin() + index), std::make_move_iterator(end()), temp.begin() + index + 1);
    	    swap(temp);
    	}
    	return begin() + index;
    }
    
    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert((pos >= begin()) && (pos < end()));
        size_t index = static_cast<size_t>(pos - begin());
        std::copy(std::make_move_iterator(begin() + index + 1), std::make_move_iterator(end()), begin() + index);
        --size_;
        return begin() + index;
    }
};
 
 
template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
 
template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}
 
template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
 
template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}
 
template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (rhs < lhs);
}
 
template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
