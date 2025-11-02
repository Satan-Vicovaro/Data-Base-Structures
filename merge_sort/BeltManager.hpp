#pragma once

#include "Belt.hpp"
#include "IOManager.hpp"
#include <memory>
#include <utility>
class BeltManager {
private:
  Belt belt_1_;
  Belt belt_2_;

public:
  BeltManager(const std::shared_ptr<IOManager> &io_manager)
      : belt_1_(io_manager, "belt_1"), belt_2_(io_manager, "belt_2") {}

  BeltManager(std::shared_ptr<IOManager> io_manager) {
    belt_1_ = Belt(io_manager, "belt_1");
    belt_2_ = Belt(io_manager, "belt_2");
  }

  void swap_belts_() { std::swap(belt_1_, belt_2_); }

  Belt &get_primary_belt() { return belt_1_; }
  Belt &get_secondary_belt() { return belt_2_; }
};
