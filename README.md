# Modern Interval Container Library (MICL)

![License](https://img.shields.io/badge/License-MIT-yellow.svg)
![Header](https://img.shields.io/badge/Header--only-Yes-brightgreen.svg)
![C++](https://img.shields.io/badge/C++-23-blue.svg)
![STL](https://img.shields.io/badge/Dependencies-STL_only-orange.svg)

## 📖 Overview

MICL (Modern Interval Container Library) is a complete, standalone redesign of Boost's Interval Container Library (ICL), built with modern C++ features and completely independent from the Boost ecosystem. This project demonstrates the application of modern C++ features (C++23 or newer versions) in reimplementing complex library components.

The library preserves the powerful functionality of Boost ICL while embracing modern C++ paradigms, resulting in cleaner syntax and enhanced type safety.

## ✨ Key Features

- 🚀 Complete Boost ICL Functionality: Full feature parity with Boost's ICL

- 📦 Header-Only Design: Single-header inclusion for seamless integration into any project

- 🛠️ Modern C++ Compliance: Fully compatible with C++23 standards

- ⚡ Zero Boost Dependencies: Completely independent from Boost libraries

- 🎯 STL-Compatible Interface: Designed to work seamlessly with Standard Template Library conventions

- 🔒 Type Safety: Improved type system with concepts and static assertions

## 🚀 Quick Start

### Prerequisites

- C++23 compatible compiler

- Standard Template Library

- Basic understanding of template metaprogramming

- Familiarity with interval mathematics concepts

### Installation

Since MICL is header-only, simply include the relevant headers in your project:

```bash
# Clone the repository
git clone https://github.com/Aurainting/Interval.git

# Copy headers to your project include directory
cp -r Interval/icl /your/project/include
```

### Basic Usage

```cpp
#include <iostream>

#include "icl/interval_set.hpp"

int main() {
  icl::interval_set<unsigned int> outages;

  outages.insert(icl::interval_set<unsigned int>::interval_type::closed(1, 9));
  outages.insert(icl::interval_set<unsigned int>::interval_type::closed(7, 10));
  outages.insert(icl::interval_set<unsigned int>::interval_type::closed(8, 11));
  outages.insert(
      icl::interval_set<unsigned int>::interval_type::closed(90, 120));

  for (const auto& i : outages) {
    std::cout << i << std::endl; // output: [1,11]\n[90,120]
  }
}
```

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

While MICL is a rewrite from first principles using modern C++ techniques, it is conceptually based on the design and algorithms of the Boost Interval Container Library (ICL). We acknowledge and respect the intellectual property of the Boost ICL authors and contributors.

The original Boost ICL is distributed under the [​​Boost Software License​​](https://boost.org.cpp.al/LICENSE_1_0.txt).

## 🙏 Acknowledgments

- Boost Community: For the original ICL design and concepts

- C++ Standards Committee: For the modern language features that made this library possible

---

⭐ If you find this project useful, please consider giving it a star on GitHub! ⭐
