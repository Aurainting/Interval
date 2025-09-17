#pragma once

namespace icl {

namespace comparison {
static constexpr int less = -1;
static constexpr int equal = 0;
static constexpr int greater = 1;
} // namespace comparison

namespace inclusion {
static constexpr int unrelated = 0;
static constexpr int subset = 1;
static constexpr int superset = 2;
static constexpr int equal = 3;
} // namespace inclusion

} // namespace icl
