#ifndef CPP98_HPP
#define CPP98_HPP

#include "boost/static_assert.hpp"

// Create static assert macro
#define STATIC_ASSERT(bool_constexpr, message) BOOST_STATIC_ASSERT_MSG(bool_constexpr, message)

#endif
