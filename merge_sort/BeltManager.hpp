#pragma once

#include "Belt.hpp"
#include <utility>
class BeltManager {
private:
  Belt belt_1_;
  Belt belt_2_;

public:
  BeltManager() {
    belt_1_ = Belt("belt_1");
    belt_2_ = Belt("belt_2");
  }

  void swap_belts_() { std::swap(belt_1_, belt_2_); }

  Belt &get_primary_belt() { return belt_1_; }
  Belt &get_secondary_belt() { return belt_2_; }
};
