#pragma once

#include "Types.h"
#include "vkr_Assert.h"

namespace vkr
{

//------------------------------------------------------------------------------
template<class T>
T vkr_max(T a, T b)
{
    return a > b ? a : b;
}

//------------------------------------------------------------------------------
template<class T>
class Array
{
public:
    //------------------------------------------------------------------------------
    Array() = default;

    //------------------------------------------------------------------------------
    ~Array()
    {
        for (int i = 0; i < count_; ++i)
            items_[i].~T();
        count_ = 0;

        capacity_ = 0;
        free(items_);
    }

    //------------------------------------------------------------------------------
    Array(const Array<T>& other)
    {
        capacity_ = other.capacity_;
        count_ = other.count_;

        items_ = (T*)malloc(sizeof(T) * capacity_);
        for (int i = 0; i < count_; ++i)
        {
            items_[i] = other.items_[i];
        }
    }

    //------------------------------------------------------------------------------
    Array<T>& operator=(const Array<T>& other)
    {
        for (int i = 0; i < count_; ++i)
            items_[i].~T();
        free(items_);

        capacity_ = other.capacity_;
        count_ = other.count_;

        items_ = (T*)malloc(sizeof(T) * capacity_);
        for (int i = 0; i < count_; ++i)
        {
            items_[i] = other.items_[i];
        }

        return *this;
    }

    //------------------------------------------------------------------------------
    Array(Array<T>&& other)
    {
        capacity_ = other.capacity_;
        count_ = other.count_;
        items_ = other.items_;
        
        other.items_ = nullptr;
        other.capacity_ = 0;
        other.count_ = 0;
    }

    //------------------------------------------------------------------------------
    Array<T>& operator=(Array<T>&& other)
    {
        for (int i = 0; i < count_; ++i)
            items_[i].~T();
        free(items_);

        capacity_ = other.capacity_;
        count_ = other.count_;
        items_ = other.items_;
        
        other.items_ = nullptr;
        other.capacity_ = 0;
        other.count_ = 0;
    }

    //------------------------------------------------------------------------------
    size_t Count() const
    {
        return count_;
    }

    //------------------------------------------------------------------------------
    const T& operator[](size_t index) const
    {
        vkr_assert(index < count_);
        return items_[index];
    }

    //------------------------------------------------------------------------------
    T& operator[](size_t index)
    {
        vkr_assert(index < count_);
        return items_[index];
    }

    //------------------------------------------------------------------------------
    void Add(const T& item)
    {
        if (count_ >= capacity_)
        {
            auto oldCapacity = capacity_;
            capacity_ = vkr_max(capacity_ << 1, MIN_CAPACITY);
            
            T* newItems = (T*)malloc(sizeof(T) * capacity_);
            memcpy(newItems, items_, sizeof(T) * oldCapacity);
            free(items_);
            items_ = newItems;
        }

        new(items_ + count_) T(item);
        ++count_;
    }

    //------------------------------------------------------------------------------
    void Insert(size_t index, const T& item)
    {
        vkr_assert(index <= count_);

        if (count_ >= capacity_)
        {
            auto oldCapacity = capacity_;
            capacity_ = vkr_max(capacity_ << 1, MIN_CAPACITY);
            
            T* newItems = (T*)malloc(sizeof(T) * capacity_);
            memcpy(newItems, items_, sizeof(T) * index);
            memcpy(&newItems[index + 1], &items_[index], (oldCapacity - index) * sizeof(T));

            free(items_);
            items_ = newItems;
        }
        else
        {
            // Move items by one to the right
            memmove(&items_[index + 1], &items_[index], (count_ - index) * sizeof(T));
        }

        new(items_ + index) T(item);
        ++count_;
    }

    //------------------------------------------------------------------------------
    void Remove(size_t index)
    {
        vkr_assert(index < count_);
        items_[index].~T();
        
        --count_;
        memmove(&items_[index], &items_[index + 1], (count_ - index) * sizeof(T));
    }

    //------------------------------------------------------------------------------
    void Clear()
    {
        for (size_t i = 0; i < count_; ++i)
        {
            items_[i].~T();
        }
        count_ = 0;
    }

    //------------------------------------------------------------------------------
    const T& Last()
    {
        vkr_assert(count_);
        return items_[count_ - 1];
    }

private:
    static constexpr size_t MIN_CAPACITY = 8;

    size_t capacity_{};
    size_t count_{};
    T* items_{};
};

}
