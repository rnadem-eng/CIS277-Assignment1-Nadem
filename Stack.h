#ifndef STACK_H
#define STACK_H

#include <cstddef>
#include <stdexcept>

template <typename T>
class Stack
{
private:
    T *elements;
    std::size_t maxCapacity;
    std::size_t count;

public:
    explicit Stack(std::size_t capacity)
        : elements(new T[capacity]),
          maxCapacity(capacity),
          count(0)
    {
    }

    ~Stack()
    {
        delete[] elements;
    }

    // Prevent copies from owning the same array.
    Stack(const Stack &) = delete;
    Stack &operator=(const Stack &) = delete;

    void push(const T &value)
    {
        if (count == maxCapacity)
        {
            throw std::overflow_error("Stack is full");
        }

        elements[count] = value;
        ++count;
    }

    T pop()
    {
        if (empty())
        {
            throw std::underflow_error("Stack is empty");
        }

        --count;
        return elements[count];
    }

    T &top()
    {
        if (empty())
        {
            throw std::underflow_error("Stack is empty");
        }

        return elements[count - 1];
    }

    bool empty() const
    {
        return count == 0;
    }

    std::size_t size() const
    {
        return count;
    }
};

#endif