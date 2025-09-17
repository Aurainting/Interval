#include "icl/interval_set.hpp"
#include <catch2/catch_test_macros.hpp>

// Here is a typical class that may model intervals in your application.
class MyInterval {
public:
  MyInterval() : _first(), _past() {}
  MyInterval(const int lo, const int up) : _first(lo), _past(up) {}
  [[nodiscard]] int first() const { return _first; }
  [[nodiscard]] int past() const { return _past; }

private:
  int _first, _past;
};

namespace icl {
// Class template interval_traits serves as adapter to register and customize
// your interval class
template <> struct interval_traits<MyInterval> {
  using interval_type = MyInterval;
  using domain_type = int;
  using domain_compare = std::less<int>;

  static interval_type construct(const domain_type& lo, const domain_type& up) {
    return {lo, up};
  }
  static domain_type lower(const interval_type& inter_val) {
    return inter_val.first();
  }
  static domain_type upper(const interval_type& inter_val) {
    return inter_val.past();
  }
};

template <> struct interval_bound_type<MyInterval> {
  using type = interval_bound_type;
  static constexpr bound_type value = interval_bounds::static_right_open;
};

} // namespace icl

TEST_CASE("Test Custom Interval", "[boost]") {
  // Now we can use class MyInterval with interval containers:
  using MyIntervalSet = icl::interval_set<int, std::less, MyInterval>;
  MyIntervalSet mySet;
  REQUIRE(mySet.empty());

  mySet += MyInterval(1, 9);
  REQUIRE(!mySet.empty());
  REQUIRE(mySet.find(1) != mySet.end());
  REQUIRE(mySet.find(9) == mySet.end());
  REQUIRE(mySet.find(5)->first() == 1);
  REQUIRE(mySet.find(6)->past() == 9);

  mySet.subtract(3) -= 6;
  REQUIRE(mySet.find(2) != mySet.end());
  REQUIRE(mySet.find(3) == mySet.end());
  REQUIRE(mySet.find(4) != mySet.end());
  REQUIRE(mySet.find(5) != mySet.end());
  REQUIRE(mySet.find(6) == mySet.end());
  REQUIRE(mySet.find(7) != mySet.end());

  mySet ^= MyInterval(2, 8);
  REQUIRE(mySet.find(2) == mySet.end());
  REQUIRE(mySet.find(3) != mySet.end());
  REQUIRE(mySet.find(4) == mySet.end());
  REQUIRE(mySet.find(5) == mySet.end());
  REQUIRE(mySet.find(6) != mySet.end());
  REQUIRE(mySet.find(7) == mySet.end());
  REQUIRE(mySet.find(8) != mySet.end());
  REQUIRE(mySet.find(9) == mySet.end());
}