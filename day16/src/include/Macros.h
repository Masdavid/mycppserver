#pragma once
#define OS_LINUX

#include <cassert>
#include <stdexcept>

// Macros to disable copying and moving
#define DISALLOW_COPY(cname)            \
  cname(const cname &) = delete;        \
  cname& operator=(const cname &) = delete;

#define DSIALLOW_MOVE(cname)            \
  cname(cname &&) = delete;             \
  cname& operator=(cname &&) = delete;  

#define DISALLOW_COPY_AND_MOVE(cname)   \
  DISALLOW_COPY(cname);                 \
  DSIALLOW_MOVE(cname);

#define ASSERT(expr, message) assert((expr) && (message))

#define UNREACHABLE(message) throw std::logic_error(message)