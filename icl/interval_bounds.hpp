#pragma once

namespace icl {

using bound_type = unsigned char;

class interval_bounds {
public:
  static constexpr bound_type static_open = 0;
  static constexpr bound_type static_left_open = 1;
  static constexpr bound_type static_right_open = 2;
  static constexpr bound_type static_closed = 3;
  static constexpr bound_type dynamic = 4;
  static constexpr bound_type undefined = 5;

  static constexpr bound_type _open = 0;
  static constexpr bound_type _left_open = 1;
  static constexpr bound_type _right_open = 2;
  static constexpr bound_type _closed = 3;

  static constexpr bound_type _right = 1;
  static constexpr bound_type _left = 2;
  static constexpr bound_type _all = 3;

public:
  interval_bounds() : _bits() {}
  explicit interval_bounds(bound_type bounds) : _bits(bounds) {}
  interval_bounds all() const { return interval_bounds(_bits & _all); }
  interval_bounds left() const { return interval_bounds(_bits & _left); }
  interval_bounds right() const { return interval_bounds(_bits & _right); }
  interval_bounds reverse_left() const {
    return interval_bounds((bound_type(~_bits) >> 1) & _right);
  }
  interval_bounds reverse_right() const {
    return interval_bounds((bound_type(~_bits) << 1) & _left);
  }

  bound_type bits() const { return _bits; }

  static interval_bounds open() { return interval_bounds(_open); }
  static interval_bounds left_open() { return interval_bounds(_left_open); }
  static interval_bounds right_open() { return interval_bounds(_right_open); }
  static interval_bounds closed() { return interval_bounds(_closed); }

public:
  bound_type _bits;
};

template <typename DomainT> class bounded_value {
public:
  using domain_type = DomainT;
  using type = bounded_value<DomainT>;

public:
  bounded_value(const domain_type& value, interval_bounds bound)
      : _value(value), _bound(bound) {}

  domain_type value() const { return _value; }
  interval_bounds bound() const { return _bound; }

private:
  domain_type _value;
  interval_bounds _bound;
};

} // namespace icl
