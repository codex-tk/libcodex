#include "gtest/gtest.h"
#include <set>

using namespace std;

int reverse(int x) {
  int sign = x < 0 ? -1 : 1;
  int64_t target = x;
  target *= sign;
  int64_t result = 0;
  while (target){
    result *= 10;
    result += (target % 10);
    target /= 10;
    if (result > 0x7fffffff)
      return 0;
  }
  return static_cast<int>(result * sign);
}
struct ListNode {
  int val;
  ListNode *next;
  ListNode(int x) : val(x), next(NULL) {}
};

ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
  set<ListNode*> set;
  while (headA) {
    set.insert(headA);
    headA = headA->next;
  }
  while (headB) {
    if (set.find(headB) != set.end())
      return headB;
    headB = headB->next;
  }
  return nullptr;
}

TEST(reverse_int, leetcode) {

  int val = reverse(-2);
  val = reverse(-2147483648);
}