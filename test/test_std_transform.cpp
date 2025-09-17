#include "icl/discrete_interval.hpp"
#include "icl/separate_interval_set.hpp"
#include "icl/split_interval_map.hpp"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <vector>

// Must include after others
#include "icl/iterator.hpp"

// Suppose we are working with a class called MyObject, containing some
// information about interval bounds e.g. _from, _to and some data members
// that carry associated information like e.g. _value.
class MyObject {
public:
  MyObject() = default;
  MyObject(const int from, const int to, const int value)
      : _from(from), _to(to), _value(value) {}
  [[nodiscard]] int from() const { return _from; }
  [[nodiscard]] int to() const { return _to; }
  [[nodiscard]] int value() const { return _value; }

private:
  int _from{};
  int _to{};
  int _value{};
};

// ... in order to use the std::transform algorithm to fill
// interval maps with MyObject data we need a function
// 'to_segment' that maps an object of type MyObject into
// the value type to the interval map we want to tranform to ...
std::pair<icl::discrete_interval<int>, int> to_segment(const MyObject& myObj) {
  return {icl::discrete_interval<int>::closed(myObj.from(), myObj.to()),
          myObj.value()};
}

// ... there may be another function that returns the interval
// of an object only
icl::discrete_interval<int> to_interval(const MyObject& myObj) {
  return icl::discrete_interval<int>::closed(myObj.from(), myObj.to());
}

// ... make_object computes a sequence of objects to test.
std::vector<MyObject> make_objects() {
  std::vector<MyObject> object_vec;
  object_vec.emplace_back(2, 3, 1);
  object_vec.emplace_back(4, 4, 1);
  object_vec.emplace_back(1, 2, 1);
  return object_vec;
}

TEST_CASE("Test Std Transform", "[boost]") {
  // This time we want to transform objects into a splitting interval map:
  icl::split_interval_map<int, int> segmap;
  std::vector<MyObject> myObjects = make_objects();

  // Use an icl::inserter to fill the interval map via inserts
  std::transform(myObjects.begin(), myObjects.end(),
                 icl::inserter(segmap, segmap.end()), to_segment);
  REQUIRE(!segmap.empty());
  REQUIRE(segmap.find(1)->first.lower() == 1);
  REQUIRE(segmap.find(1)->first.upper() == 2);
  REQUIRE(segmap.find(1)->second == 1);

  REQUIRE(segmap.find(2)->first.lower() == 2);
  REQUIRE(segmap.find(3)->first.upper() == 3);
  REQUIRE(segmap.find(3)->second == 1);

  REQUIRE(segmap.find(4)->first.lower() == 4);
  REQUIRE(segmap.find(4)->first.upper() == 4);
  REQUIRE(segmap.find(4)->second == 1);
  REQUIRE(segmap.find(5) == segmap.end());

  segmap.clear();
  REQUIRE(segmap.empty());

  // In order to compute aggregation results on associated values, we
  // usually want to use an icl::adder instead of an std or icl::inserter
  std::transform(myObjects.begin(), myObjects.end(),
                 icl::adder(segmap, segmap.end()), to_segment);
  REQUIRE(!segmap.empty());
  REQUIRE(segmap.find(1)->first.lower() == 1);
  REQUIRE(segmap.find(1)->first.upper() == 2);
  REQUIRE(segmap.find(1)->second == 1);

  REQUIRE(segmap.find(2)->first.lower() == 2);
  REQUIRE(segmap.find(2)->first.upper() == 2);
  REQUIRE(segmap.find(2)->second == 2);
  REQUIRE(segmap.find(3)->first.lower() == 2);
  REQUIRE(segmap.find(3)->first.upper() == 3);
  REQUIRE(segmap.find(3)->second == 1);

  REQUIRE(segmap.find(4)->first.lower() == 4);
  REQUIRE(segmap.find(4)->first.upper() == 4);
  REQUIRE(segmap.find(4)->second == 1);
  REQUIRE(segmap.find(5) == segmap.end());

  icl::separate_interval_set<int> segset;
  std::transform(myObjects.begin(), myObjects.end(),
                 icl::adder(segset, segset.end()), to_interval);
  REQUIRE(!segset.empty());
  REQUIRE(segset.find(1)->lower() == 1);
  REQUIRE(segset.find(1)->upper() == 3);
  REQUIRE(segset.find(2)->lower() == 1);
  REQUIRE(segset.find(3)->upper() == 3);
  REQUIRE(segset.find(4)->lower() == 4);
  REQUIRE(segset.find(4)->upper() == 4);
}