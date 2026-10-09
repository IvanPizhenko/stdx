// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, Ivan Pizhenko. All rights reserved.

// Project
#include "../../include/ptr_unique_queue.hpp"

// STL
#include <iostream>

template <typename Q, typename Arg>
concept can_push_back = requires(Q& q, Arg&& arg) {
    q.push_back(std::forward<Arg>(arg));
};

static_assert(can_push_back<stdx::ptr_unique_queue<int, std::unique_ptr<int>>, std::unique_ptr<int>>, "push_back(rvalue) should be enabled for unique_ptr");
static_assert(!can_push_back<stdx::ptr_unique_queue<int, std::unique_ptr<int>>, const std::unique_ptr<int>&>, "push_back(const lvalue) should be disabled for unique_ptr");
static_assert(!can_push_back<stdx::ptr_unique_queue<int, std::unique_ptr<int>>, std::unique_ptr<int>&>, "push_back(lvalue) should be disabled for unique_ptr");

static_assert(can_push_back<stdx::ptr_unique_queue<int, std::shared_ptr<int>>, std::shared_ptr<int>>, "push_back(rvalue) should be enabled for shared_ptr");
static_assert(can_push_back<stdx::ptr_unique_queue<int, std::shared_ptr<int>>, const std::shared_ptr<int>&>, "push_back(const lvalue) should be enabled for shared_ptr");
static_assert(can_push_back<stdx::ptr_unique_queue<int, std::shared_ptr<int>>, std::shared_ptr<int>&>, "push_back(lvalue) should be enabled for shared_ptr");

void uq_test1()
{
    stdx::ptr_unique_queue<int, std::unique_ptr<int>> q(2);
    for (int i = 0; i < 100; ++i) {
        int v = i % 10;
        q.push_back(std::make_unique<int>(v));
    }
    std::cout << std::endl << "========================================================" << std::endl;
    for (const auto& e: q) {
        std::cout << '[' << e.id << ']' << ' ' << *e.payload << std::endl;
    }
    std::cout << "========================================================" << std::endl;
}

void uq_test2()
{
    stdx::ptr_unique_queue<int, std::shared_ptr<int>> q(1);
    for (int i = 0; i < 100; ++i) {
        int v = i % 10;
        auto p = std::make_shared<int>(v);
        q.push_back(p);
    }
    std::cout << std::endl << "========================================================" << std::endl;
    for (const auto& e: q) {
        std::cout << '[' << e.id << ']' << ' ' << *e.payload << std::endl;
    }
    std::cout << "========================================================" << std::endl;
}

int main()
{
    uq_test1();
    uq_test2();
    return 0;
}
