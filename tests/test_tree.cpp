#include <gtest/gtest.h>

namespace {
  template< typename typeT >
  struct TreeNode{
    typeT value;
    TreeNode* left;
    TreeNode* right;
    TreeNode( typeT&& val )
      : value( std::forward<typeT>(val))
      , left(nullptr)
      , right(nullptr)
      {}
  };
}

TEST( tree , init ){
  TreeNode<int> root(0);

}
