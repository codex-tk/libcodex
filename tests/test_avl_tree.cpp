#include <gtest/gtest.h>

namespace {
  template< typename typeT >
  struct avl_tree_node{
    typeT value;
    int balance_factor;
    avl_tree_node* left;
    avl_tree_node* right;
    avl_tree_node( typeT&& val )
      : value( std::forward<typeT>(val))
      , balance_factor(0)
      , left(nullptr)
      , right(nullptr)
      {}
  };
}

TEST( avl_tree , init ){
  avl_tree_node<int> root(0);

}
