// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2024-2025, Ivan Pizhenko. All rights reserved.

// Project
#include "../../include/unique_queue.hpp"

// STL
#include <iostream>

void uq_test1()
{
  stdx::unique_queue<int> q(2);
  for (int i = 0; i < 100; ++i) {
    int v = i % 10;
    q.push_back(v);
  }
  std::cout << std::endl << "========================================================" << std::endl;
  for (const auto& e: q) {
    std::cout << '[' << e.id << ']' << ' ' << e.payload << std::endl;
  }
  std::cout << "========================================================" << std::endl;
}

void uq_test2()
{
  stdx::unique_queue<int> q(1);
  for (int i = 0; i < 100; ++i) {
    int v = i % 10;
    q.push_back(v);
  }
  std::cout << std::endl << "========================================================" << std::endl;
  for (const auto& e: q) {
    std::cout << '[' << e.id << ']' << ' ' << e.payload << std::endl;
  }
  std::cout << "========================================================" << std::endl;
}

int main()
{
    uq_test1();
    uq_test2();
    return 0;
}
