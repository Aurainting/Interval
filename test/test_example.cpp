#include "icl/closed_interval.hpp"
#include "icl/continuous_interval.hpp"
#include "icl/discrete_interval.hpp"
#include "icl/dynamic_interval_traits.hpp"
#include "icl/functors.hpp"
#include "icl/interval.hpp"
#include "icl/interval_base_map.hpp"
#include "icl/interval_base_set.hpp"
#include "icl/interval_bounds.hpp"
#include "icl/interval_combining_style.hpp"
#include "icl/interval_map.hpp"
#include "icl/interval_set.hpp"
#include "icl/interval_traits.hpp"
#include "icl/iterator.hpp"
#include "icl/left_open_interval.hpp"
#include "icl/map.hpp"
#include "icl/open_interval.hpp"
#include "icl/right_open_interval.hpp"
#include "icl/separate_interval_set.hpp"
#include "icl/split_interval_map.hpp"
#include "icl/split_interval_set.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test Basic Usage", "[basic]") {
  icl::interval_set<unsigned int> outages;

  outages.insert(icl::interval_set<unsigned int>::interval_type::closed(1, 9));
  outages.insert(icl::interval_set<unsigned int>::interval_type::closed(7, 10));
  outages.insert(icl::interval_set<unsigned int>::interval_type::closed(8, 11));
  outages.insert(
      icl::interval_set<unsigned int>::interval_type::closed(90, 120));

  REQUIRE(outages.begin() != outages.end());
  REQUIRE(!outages.empty());
  REQUIRE(outages.size() > 1);

  REQUIRE(outages.find(2)->lower() == 1);
  REQUIRE(outages.find(2)->upper() == 11);

  REQUIRE(outages.find(100)->lower() == 90);
  REQUIRE(outages.find(100)->upper() == 120);
}