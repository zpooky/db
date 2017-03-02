#include "gtest/gtest.h"
#include "../collection/BinaryTree.h"
#include<iostream>
#include <set>

using std::cout;
using std::string;

TEST(BinaryTree, test) {
    sp::BinaryTree<size_t> bt(size_t(1));
    auto res = bt.insert(2);
}
