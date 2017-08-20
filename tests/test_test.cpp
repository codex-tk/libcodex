#include <vector>
#include "gtest/gtest.h"
#include "test_helper.hpp"

namespace unittest {
  struct test_base{
    test_base( void ){
      test_base::tests.push_back(this);
    }
    virtual void operator()(void) = 0;

    static void run_all_tests( void ) {
      for( test_base* tb : test_base::tests ) {
        (*tb)();
      }
    }
    static std::vector<test_base*> tests;

    template <typename A0 , typename ...Args >
    static void print( A0&& a0 , Args&&... args ) {
      std::cout << a0 << ":";
      test_base::print( std::forward<Args>(args)...);
    }

    template <typename A0 >
    static void print( A0&& a0  ) {
      std::cout << a0 << std::endl;
    }
  };
  std::vector<test_base*> test_base::tests;

#ifndef SAMPLE_ASSERT_EQ
#define SAMPLE_ASSERT_EQ( a , b ) if ( a != b ) unittest::test_base::print( __FILE__ , __LINE__ , "FAIL COMPARE EQ" );
#endif
#ifndef DECLARE_TEST
#define DECLARE_TEST( name ) class test_impl_##name : public unittest::test_base { virtual void operator()( void ) override; } _test_impl_##name; void test_impl_##name::operator()( void )
#endif
}




DECLARE_TEST(sample){
  SAMPLE_ASSERT_EQ( 1 , 2 );
}

using namespace std;

vector<int> twoSum(vector<int> nums, int target) {
    map<int,int> val_idx;
    for( int i = 0 ; i < nums.size() ; ++i ) {
        int val = target - nums[i];
        if ( val_idx.find(val) != val_idx.end() )
            return { i , val_idx[val] };
        val_idx[nums[i]] = i;
    }
    return {0,0};
}

int lengthOfLongestSubstring(string s) {
  map<char,int> ch_idx;
  int start = 0;
  int end = 0;
  int max_length = 0;
  for( int i = 0 ; i < s.length() ; ++i ) {
      auto it = ch_idx.find(s[i]);
      if ( it == ch_idx.end() || it->second < start ) {
          end = i + 1;
          int len = end - start;
          if ( len > max_length )
              max_length = len;
      }else{
          start = ch_idx[s[i]] + 1;
      }
      ch_idx[s[i]] = i;
  }
  return max_length;
}
int hammingDistance(int x, int y) {
    int len = sizeof(int) * 8;
    int dist = 0;
    for ( int i = 0 ; i < len ; ++i ) {
        int mask = 1 << i;
        if ( x & mask != y & mask )
            ++dist;
    }
    return dist;
}
TEST( tk_test_run , run ) {
  unittest::test_base::run_all_tests();
  auto val = twoSum({3,2,4}, 6 );
  lengthOfLongestSubstring("pwwkew");
  hammingDistance(1,4);
}
