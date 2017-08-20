#include "gtest/gtest.h"
using namespace std;

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}

};

TEST(complement, leetcode) {
  class solution {
  public:
    static int complement(int num) {
      int mask = 0;
      while (mask < num) {
        mask <<= 1;
        mask |= 0x01;
      }
      return mask & ~num;
    }
  };

  int val = solution::complement(5);
}

TEST(bit_count, leetcode) {
  class solution {
  public:
    static int bit_count(int n) {
      int count = 0;
      while (n) {
        // remove last bit
        // 1111 -> 1110 -> 1100 -> 1000 -> 0000
        int and = n - 1;
        n = n & and;
        ++count;
      }
      return count;
    }

    static int getSum(int a, int b) {
      return b == 0 ? a : getSum(a^b, (a&b) << 1);
    }

    static uint32_t reverseBits(uint32_t n) {
      uint32_t mask = 1, ret = 0;
      for (int i = 0; i < 32; ++i) {
        ret <<= 1;
        if (mask & n) ret |= 1;
        mask <<= 1;
      }
      return ret;
    }
    static int addDigits(int num) {
      return 1 + (num - 1) % 9;
    }

    static int nthUglyNumber(int n) {
      std::vector< int > ugly_numbers;
      ugly_numbers.push_back(1);
      int n2 = 0;
      int n3 = 0;
      int n5 = 0;
      while (ugly_numbers.size() < n) {
        int n2_val = ugly_numbers[n2] * 2;
        int n3_val = ugly_numbers[n3] * 3;
        int n5_val = ugly_numbers[n5] * 5;
        int min_val = std::min(n2_val, std::min(n3_val, n5_val));
        ugly_numbers.push_back(min_val);
        if (min_val == n2_val) n2++;
        if (min_val == n3_val) n3++;
        if (min_val == n5_val) n5++;
      }
      return ugly_numbers[n - 1];
    }

    static bool inorder_i(TreeNode* node) {
      if (node == nullptr)
        return true;
      bool init = false;
      int min_value = 0;
      std::vector< TreeNode* > stack;
      while (node != nullptr || !stack.empty()) {
        while (node) {
          stack.push_back(node);
          node = node->left;
        }
        node = stack.back(); stack.pop_back();
        if (!init) {
          init = true;
        }
        else {
          if (min_value >= node->val) return false;
        }
        min_value = node->val;
        node = node->right;
      }
      return true;
    }
  };
  int bitcount = solution::bit_count(0xf);
  int sum = solution::getSum(1, 2);
  uint32_t rv = solution::reverseBits(0x01);
  int add_digit = solution::addDigits(1853);
  solution::nthUglyNumber(7);

  TreeNode left(1);
  TreeNode root(1);
  root.left = &left;
  solution::inorder_i(&root);
}

TEST(complex, leetcode) {
  class Solution {
  public:
    enum class term_type {
      k_constant,
      k_complex,
    };
    struct term {
      term_type type;
      int coef;

      term(int c, term_type t)
        : coef(c), type(t)
      {}

      term operator*(const term& rhs) {
        if (type == term_type::k_constant && rhs.type == term_type::k_constant) {
          return term(coef * rhs.coef, term_type::k_constant);
        }
        if (type == term_type::k_complex && rhs.type == term_type::k_complex) {
          return term(coef * rhs.coef * -1, term_type::k_constant);
        }
        return term(coef * rhs.coef, term_type::k_complex);
      }
    };

    struct poly {
      std::vector< term > value;

      static int atoi(const string& str) {
        bool minus = false;
        int value = 0;
        int index = 0;
        if (str[index] == '+') ++index;
        if (str[index] == '-') {
          minus = true;
          ++index;
        }
        while (index < str.size()) {
          value *= 10;
          value += str[index++] - '0';
        }
        if (minus) value *= -1;
        return value;
      }

      poly(const string& str) {
        int value = 0;
        bool minus = false;
        string term_str;
        for (char ch : str) {
          if (ch == '+' || ch == '-') {
            if (term_str.size() == 1 && term_str[0] == '+') term_str.clear();
            if (term_str.size() != 0) {
              Solution::term t(poly::atoi(term_str), term_type::k_constant);
              this->value.push_back(t);
              term_str.clear();
            }
          }
          if (ch == 'i') {
            Solution::term t(poly::atoi(term_str), term_type::k_complex);
            this->value.push_back(t);
          }
          term_str.push_back(ch);
        }
      }

      poly operator*( poly& rhs) {
        poly p("");
        for (term& t : this->value) {
          for (term& t2 : rhs.value) {
            p.value.push_back(t * t2);
          }
        }
        int con = 0;
        int com = 0;
        for (term& t : p.value)
          if (t.type == term_type::k_constant)
            con += t.coef;
          else
            com += t.coef;

        p.value.clear();
        p.value.push_back(term(con, term_type::k_constant));
        p.value.push_back(term(com, term_type::k_complex));
        return p;
      }

      static void reverse(string& s) {
        int b = 0; int e = s.size() - 1;
        while (b < e) {
          swap(s[b++], s[e--]);
        }
      }

      static string itoa(int val, int r) {
        if (val == 0) return "0";
        static string map("0123456789ABCEDF");
        if (r < 2 || r > 16) return "";
        bool minus = val < 0 ? true : false;
        if (minus) val *= -1;
        string sval;
        while (val) {
          sval.push_back(map[val % r]);
          val /= r;
        }
        if (minus) sval.push_back('-');
        reverse(sval);
        return sval;
      }

      string to_string() {
        bool first = true;
        string p;
        for (term& t : this->value) {
          if (!first) {
            p.push_back('+');
          }
          p += itoa(t.coef, 10);
          if (t.type == term_type::k_complex)
            p.push_back('i');
          first = false;
        }
        return p;
      }
    };


    static string complexNumberMultiply(string a, string b) {
      poly pa(a);
      poly pb(b);
      poly result = pa * pb;
      return result.to_string();
    }
  };
  string s = Solution::complexNumberMultiply("1+1i", "1+1i");
  string s1 = Solution::complexNumberMultiply("1+-1i", "1+-1i");

  string s2 = Solution::complexNumberMultiply("78+-76i", "-86+72i");
}

TEST(match, leetcode) {
  struct Solution {
    static bool isMatch(string s, string p) {
      int m = s.length() + 1;
      int n = p.length() + 1;
      // dp map
      vector<vector<bool>> dp(m, std::vector<bool>(n, false));

      // "" , "" is match
      dp[0][0] = true;

      // in this case can't start with '*'
      for (int j = 1; j < n; ++j) {
        if (p[j - 1] == '*' && j > 1) {
          dp[0][j] = dp[0][j - 2];
        }
      }

      // dp
      for (int i = 1; i < m; ++i) {
        for (int j = 1; j < n; ++j) {
          // if s[n] == p[n] ( p[n] == '.' is matches all single char )
          // check s[n-1 ... 0] == p[n-1 ... 0 ]
          if ((s[i - 1] == p[j - 1]) || (p[j - 1] == '.')) {
            dp[i][j] = dp[i - 1][j - 1];
          }
          // if pattern is '*'
          if (p[j - 1] == '*') {
            // check pattern is empty -> s[n ... 0] == p[n-2 ... 0]
            bool empty = dp[i][j - 2];
            // check pattern repeated s[n] == p[n-1] (start ch! or '.')  && s[n-1 ... 0] == p[n ... 0]
            bool repeat = ((s[i - 1] == p[j - 2]) || (p[j - 2] == '.')) && dp[i - 1][j];
            dp[i][j] = empty || repeat;
          }
        }
      }
      return dp[m - 1][n - 1];
    }
  };
  Solution::isMatch("aab", "c*a*b");
}

TEST(post_order, leetcode) {
  /**
  * Definition for a binary tree node.
  * struct TreeNode {
  *     int val;
  *     TreeNode *left;
  *     TreeNode *right;
  *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
  * };
  */
  class Solution {
  public:
    map< int, int > sum_cnt;

    vector<int> post_order_i(TreeNode* node) {
      vector<int> order;
      vector<TreeNode*> stack;
      int v = 0;
      while (node || !stack.empty()) {
        while (node) {
          if (node->right)
            stack.push_back(node->right);
          stack.push_back(node);
          node = node->left;
        }
        node = stack.back(); stack.pop_back();

        if (node->right && stack.back() == node->right) {
          stack.pop_back();
          stack.push_back(node);
          node = node->right;
        }
        else {
          // use node
          node = nullptr;
        }
      }
      return order;
    }

    int post_order(TreeNode* node) {
      if (node) {
        int l = post_order(node->left);
        int r = post_order(node->right);
        int v = l + r + node->val;
        if (sum_cnt.find(v) != sum_cnt.end()) {
          sum_cnt[v] += 1;
        }
        else {
          sum_cnt[v] = 1;
        }
        return v;
      }
      return 0;
    }
    vector<int> findFrequentTreeSum(TreeNode* root) {
      return post_order_i(root);
      int max_cnt = 0;
      for (auto it : sum_cnt) {
        if (it.second > max_cnt)
          max_cnt = it.second;
      }
      vector<int> ans;
      for (auto it : sum_cnt) {
        if (it.second == max_cnt)
          ans.push_back(it.first);
      }
      return ans;
    }

    static bool isMatch(string s, string p) {
      int m = s.length() + 1;
      int n = p.length() + 1;
      vector<vector<int>> dp;
      for (int i = 0; i < m; ++i) {
        dp.push_back(vector<int>(n, 0));
      }

      dp[0][0] = 1;

      for (int j = 1; j < m; ++j) {
        if (p[j - 1] == '*' && j > 1)
          dp[0][j] = dp[0][j - 2];
      }

      for (int i = 1; i < m; ++i) {
        for (int j = 1; j < n; ++j) {
          if ((s[i - 1] == p[j - 1]) || (p[j - 1] == '.')) {
            dp[i][j] = dp[i - 1][j - 1];
          }
          if (p[j - 1] == '*') {
            bool empty = dp[i][j - 2];
            bool repeat = ((s[i - 1] == p[j - 2]) || (p[j - 2] == '.')) && dp[i - 1][j];
            dp[i][j] = empty || repeat;
          }
        }
      }
      return dp[m][n];
    }
  };

}

TEST(next_greate_number, leetcode) {
  class Solution {
  public:
    static     vector<int> nextGreaterElements(vector<int>&& nums) {
      map<int, int> next_greater;
      vector<int> stack;

      for (int i = 0; i < nums.size(); ++i) {
        stack.clear();
        for (int j = i; j < i + nums.size(); ++j) {
          int real_index = j % nums.size();
          while (!stack.empty() && stack.back() < nums[real_index]) {
            if (next_greater.find(stack.back()) == next_greater.end())
              next_greater[stack.back()] = nums[real_index];
            stack.pop_back();
          }
          stack.push_back(nums[real_index]);
        }
      }
      vector<int> ans(nums.size(), -1);
      for (int i = 0; i < nums.size(); ++i) {
        auto it = next_greater.find(nums[i]);
        if (it != next_greater.end()) {
          ans[i] = it->second;
        }
      }
      return ans;
    }

    static int nextGreaterElement2(int n) {
      string digits = to_string(n);

      int index = 0;

      // find index of bigger then befor digit
      for (int i = digits.size() - 1; i > 0; --i) {
        if (digits[i] > digits[i - 1]) {
          index = i;
          break;
        }
      }
      // desending order can't find 
      if (index == 0)
        return -1;

      int target = index - 1;
      int swap_target = index;
      // find index of smallest digit ( but bigger then target )
      for (int i = index; i < digits.size(); ++i) {
        if ((digits[i] > digits[target]) && (digits[i] < digits[swap_target])) {
          swap_target = i;
        }
      }
      swap(digits[target], digits[swap_target]);

      for (int i = index; i < digits.size(); ++i) {
        for (int j = i + 1; j < digits.size(); ++j) {
          if (digits[i] > digits[j]) {
            swap(digits[i], digits[j]);
          }
        }
      }
      return atoi(digits.c_str());

    }
  };
  
  Solution::nextGreaterElements({ 100,1,11,1,120,111,123,1,-1,-100 });
  Solution::nextGreaterElement2(1999999999);
}

TEST(atoi, leetcode) {
  struct Solution {
    static int myAtoi(string str) {
      int sign = 1;
      int64_t val = 0;
      int sign_count = 0;
      bool started = false;
      for (char ch : str) {
        if (ch == ' ') {
          if (!started)
            continue;
          break;
        }
        if (ch == '+') {
          sign_count += 1;
          sign *= 1;
          started = true;
          continue;
        }
        if (ch == '-') {
          sign_count += 1;
          sign *= -1;
          started = true;
          continue;
        }
        if (sign_count > 1)
          return 0;
        if (ch >= '0' && ch <= '9') {
          val *= 10;
          val += ch - '0';
          started = true;
          continue;
        }
        break;
      }
      val *= sign;
      if (val > 0x7fffffff) return 0x7fffffff;
      int64_t v2 = (~0x7fffffff);
      if (val < v2)
        return v2;
      return static_cast<int>(val);
    }
  };
  int val = Solution::myAtoi("-2147483648");
}