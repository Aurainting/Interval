#include "icl/discrete_interval.hpp"
#include "icl/interval_map.hpp"
#include <catch2/catch_test_macros.hpp>

/* The most simple example of an interval_map is an overlap counter.
   If intervals are added that are associated with the value 1,
   all overlaps of added intervals are counted as a result in the
   associated values.
*/
using OverlapCounterT = icl::interval_map<int, int>;

TEST_CASE("Test Overlap Counter", "[boost]") {
  OverlapCounterT overlap_counter;

  icl::discrete_interval<int> inter_val =
      icl::discrete_interval<int>::right_open(4, 8);
  REQUIRE(inter_val.lower() == 4);
  REQUIRE(inter_val.upper() == 8);

  overlap_counter += make_pair(inter_val, 1);
  REQUIRE(!overlap_counter.empty());
  REQUIRE(overlap_counter.find(4) != overlap_counter.end());
  REQUIRE(overlap_counter.find(7) != overlap_counter.end());
  REQUIRE(overlap_counter.find(8) == overlap_counter.end());
  REQUIRE(overlap_counter.find(5)->first.lower() == 4);
  REQUIRE(overlap_counter.find(6)->first.upper() == 8);
  REQUIRE(overlap_counter.find(7)->second == 1);

  inter_val = icl::discrete_interval<int>::right_open(6, 9);
  overlap_counter += make_pair(inter_val, 1);
  REQUIRE(overlap_counter.find(6) != overlap_counter.end());
  REQUIRE(overlap_counter.find(9) == overlap_counter.end());
  REQUIRE(overlap_counter.find(7)->first.lower() == 6);
  REQUIRE(overlap_counter.find(8)->first.upper() == 9);
  REQUIRE(overlap_counter.find(6)->second == 2);
  REQUIRE(overlap_counter.find(7)->second == 2);
  REQUIRE(overlap_counter.find(8)->second == 1);

  inter_val = icl::discrete_interval<int>::right_open(1, 9);
  overlap_counter += make_pair(inter_val, 1);
  REQUIRE(overlap_counter.find(1) != overlap_counter.end());
  REQUIRE(overlap_counter.find(2)->first.lower() == 1);
  REQUIRE(overlap_counter.find(3)->first.upper() == 4);
  REQUIRE(overlap_counter.find(4)->second == 2);
  REQUIRE(overlap_counter.find(5)->second == 2);
  REQUIRE(overlap_counter.find(6)->second == 3);
  REQUIRE(overlap_counter.find(8)->second == 2);
}
