#include "SuperDataBase.hpp"
#include <ctime>
#include <random>

int main(int argc, char *argv[]) {
  std::mt19937 mt(std::time(nullptr));
  SuperDataBase db = SuperDataBase();
  db.start();
}
