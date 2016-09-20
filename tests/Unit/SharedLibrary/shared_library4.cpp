
// RUN: %hc -fPIC -Wl,-Bsymbolic -shared -DSHARED_LIBRARY %s -o %T/libtest4.so
// RUN: %hc %s -L%T -ltest4 -o %t.out && LD_LIBRARY_PATH=%T %t.out

// kernels built as one single shared libary
// linked dynamically with the main program which contains
// direct kernel invocations

#if SHARED_LIBRARY

#include <hc.hpp>

extern "C" int foo(int grid_size) {
  using namespace hc;

  extent<1> ex(grid_size);
  array_view<int, 1> av(grid_size);

  parallel_for_each(ex, [=](index<1>& idx) [[hc]] {
    av(idx) = 1;
  }).wait();

  int ret = 0;
  for (int i = 0; i < grid_size; ++i) {
    ret += av[i];
  }

  return ret;
}

extern "C" int bar(int grid_size) {
  using namespace hc;

  extent<1> ex(grid_size);
  array_view<int, 1> av(grid_size);

  parallel_for_each(ex, [=](index<1>& idx) [[hc]] {
    av(idx) = 2;
  }).wait();

  int ret = 0;
  for (int i = 0; i < grid_size; ++i) {
    ret += av[i];
  }

  return ret;
}

#else

#include <hc.hpp>

extern "C" int foo(int);
extern "C" int bar(int);

int baz(int grid_size) {
  using namespace hc;

  extent<1> ex(grid_size);
  array_view<int, 1> av(grid_size);
  parallel_for_each(ex, [=](index<1>& idx) [[hc]] {
    av(idx) = 3;
  }).wait();

  int ret = 0;
  for (int i = 0; i < grid_size; ++i) {
    ret += av[i];
  }
  return ret;
}

int main() {
  bool ret = true;

  for (int i = 1; i <= 16; ++i) {
    ret &= (foo(i) == i);
    ret &= (bar(i * 2) == (i * 4));
  }

  for (int i = 1; i <= 16; ++i) {
    ret &= (baz(i * 3) == (i * 9));
  }

  for (int i = 1; i <= 16; ++i) {
    ret &= (foo(i) == i);
    ret &= (bar(i * 2) == (i * 4));
  }

  for (int i = 1; i <= 16; ++i) {
    ret &= (baz(i * 3) == (i * 9));
  }

  return !(ret == true);
}

#endif
