#include <vector>
#include <array>



template <int C>
class A {
public:
  template <int CC>
  consteval A<CC> set() const { return A<CC>(); }
  consteval int ff() const { return C - 1; }
};

consteval void f() {
  A<12> a;
  int b = 11;
  a.set<b>();
}

int main() {
  f();
}