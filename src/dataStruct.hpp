#ifndef DATASTRUCT_H_
#define DATASTRUCT_H_
#pragma once

#include <vector>

template <typename T> class IndexableStack {
private:
  std::vector<T> elements;

public:
  void push(const T &value) { elements.push_back(value); }

  void pop() {
    if (elements.empty()) {
      throw std::out_of_range("Stack is empty");
    }
    elements.pop_back();
  }

  T &top() {
    if (elements.empty()) {
      throw std::out_of_range("Stack is empty");
    }
    return elements.back();
  }

  const T &top() const {
    if (elements.empty()) {
      throw std::out_of_range("Stack is empty");
    }
    return elements.back();
  }

  T &get(size_t index) {
    if (index >= elements.size()) {
      throw std::out_of_range("Index out of range");
    }
    return elements[index];
  }

  const T &get(size_t index) const {
    if (index >= elements.size()) {
      throw std::out_of_range("Index out of range");
    }
    return elements[index];
  }

  bool empty() const { return elements.empty(); }

  size_t size() const { return elements.size(); }
};

#endif // DATASTRUCT_H_
