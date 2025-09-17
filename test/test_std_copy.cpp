#include "icl/discrete_interval.hpp"
#include "icl/interval_map.hpp"
#include "icl/iterator.hpp"
#include <catch2/catch_test_macros.hpp>
#include <vector>

// 'make_segments' returns a vector of interval value pairs, which
// are not sorted. The values are taken from the minimal example
// in section 'interval combining styles'.
std::vector<std::pair<icl::discrete_interval<int>, int>> make_segments() {
  std::vector<std::pair<icl::discrete_interval<int>, int>> segment_vec;
  segment_vec.emplace_back(icl::discrete_interval<int>::right_open(2, 4), 1);
  segment_vec.emplace_back(icl::discrete_interval<int>::right_open(4, 5), 1);
  segment_vec.emplace_back(icl::discrete_interval<int>::right_open(1, 3), 1);
  return segment_vec;
}

// 'show_segments' displays the source segments.
void show_segments(
    const std::vector<std::pair<icl::discrete_interval<int>, int>>& segments) {
  auto iter = segments.begin();
  while (iter != segments.end()) {
    ++iter;
  }
}

TEST_CASE("Test Std Copy", "[boost]") {
  // So we have some segments stored in an std container.
  std::vector<std::pair<icl::discrete_interval<int>, int>> segments =
      make_segments();
  REQUIRE(!segments.empty());
  REQUIRE(segments.size() == 3);
  REQUIRE(segments[0].first.lower() == 2);
  REQUIRE(segments[0].first.upper() == 4);
  REQUIRE(segments[0].second == 1);

  // We are going to 'std::copy' those segments into an interval_map:
  icl::interval_map<int, int> segmap;

  // Use an 'icl::inserter' from <boost/icl/iterator.hpp> to call
  // insertion on the interval container.
  std::copy(segments.begin(), segments.end(),
            icl::inserter(segmap, segmap.end()));
  REQUIRE(!segmap.empty());
  REQUIRE(segmap.find(2) != segmap.end());
  REQUIRE(segmap.find(4) != segmap.end());
  REQUIRE(segmap.find(4)->first.lower() == 1);
  REQUIRE(segmap.find(3)->first.upper() == 5);
  REQUIRE(segmap.find(4)->second == 1);
  segmap.clear();
  REQUIRE(segmap.empty());

  // When we are feeding data into interval_maps, most of the time we are
  // intending to compute an aggregation result. So we are not interested
  // the std::insert semantincs but the aggregating icl::addition semantics.
  // To achieve this there is an icl::add_iterator and an icl::adder function
  // provided in <boost/icl/iterator.hpp>.
  std::copy(segments.begin(), segments.end(),
            icl::adder(segmap, segmap.end())); // Aggregating associated values

  REQUIRE(!segmap.empty());
  REQUIRE(segmap.find(1)->first.lower() == 1);
  REQUIRE(segmap.find(1)->first.upper() == 2);
  REQUIRE(segmap.find(1)->second == 1);
  REQUIRE(segmap.find(2)->first.lower() == 2);
  REQUIRE(segmap.find(2)->first.upper() == 3);
  REQUIRE(segmap.find(2)->second == 2);
  REQUIRE(segmap.find(3)->first.lower() == 3);
  REQUIRE(segmap.find(4)->first.upper() == 5);
  REQUIRE(segmap.find(4)->second == 1);
  REQUIRE(segmap.find(5) == segmap.end());

  // In this last case, the semantics of 'std::copy' transforms to the
  // generalized addition operation, that is implemented by operator
  // += or + on itl maps and sets.
}