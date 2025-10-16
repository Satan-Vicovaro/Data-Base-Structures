#pragma once
#include <cstring>
#include <iostream>

class Object1 {
private:
  int num1_;
  float num2_;
  char somestring_[16];

public:
  Object1() {
    num1_ = 1234;
    num2_ = 2137.0;
    std::strcpy(somestring_, "Hello world");
  }

  Object1(const Object1 &other) {
    num1_ = other.num1_;
    num2_ = other.num2_;
    std::strcpy(somestring_, other.somestring_);
  }

  friend std::ostream &operator<<(std::ostream &os, const Object1 &obj) {
    os << "num1: " << obj.num1_ << ", num2: " << obj.num2_
       << ", somestring: " << obj.somestring_;
    return os;
  }

  Object1 &operator=(const Object1 &other) {
    if (this == &other) {
      return *this;
    }
    return *this;
  }
};
