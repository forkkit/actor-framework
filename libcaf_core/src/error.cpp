/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2018 Dominik Charousset                                     *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#include "caf/error.hpp"

#include "caf/actor_system_config.hpp"
#include "caf/config.hpp"
#include "caf/deep_to_string.hpp"
#include "caf/deserializer.hpp"
#include "caf/message.hpp"
#include "caf/serializer.hpp"

namespace caf {

// -- nested classes -----------------------------------------------------------

struct error::data {
  uint8_t code;
  uint8_t category;
  message context;
};

// -- constructors, destructors, and assignment operators ----------------------

error::error() noexcept : data_(nullptr) {
  // nop
}

error::error(none_t) noexcept : data_(nullptr) {
  // nop
}

error::error(error&& x) noexcept : data_(x.data_) {
  if (data_ != nullptr)
    x.data_ = nullptr;
}

error& error::operator=(error&& x) noexcept {
  if (this != &x)
    std::swap(data_, x.data_);
  return *this;
}

error::error(const error& x) : data_(x ? new data(*x.data_) : nullptr) {
  // nop
}

error& error::operator=(const error& x) {
  if (this == &x)
    return *this;
  if (x) {
    if (data_ == nullptr)
      data_ = new data(*x.data_);
    else
      *data_ = *x.data_;
  } else {
    clear();
  }
  return *this;
}

error::error(uint8_t x, uint8_t y)
  : data_(x != 0 ? new data{x, y, none} : nullptr) {
  // nop
}

error::error(uint8_t x, uint8_t y, message z)
  : data_(x != 0 ? new data{x, y, std::move(z)} : nullptr) {
  // nop
}

error::~error() {
  delete data_;
}

// -- observers ----------------------------------------------------------------

uint8_t error::code() const noexcept {
  CAF_ASSERT(data_ != nullptr);
  return data_->code;
}

uint8_t error::category() const noexcept {
  CAF_ASSERT(data_ != nullptr);
  return data_->category;
}

const message& error::context() const noexcept {
  CAF_ASSERT(data_ != nullptr);
  return data_->context;
}

int error::compare(const error& x) const noexcept {
  uint8_t x_code;
  uint8_t x_category;
  if (x) {
    x_code = x.data_->code;
    x_category = x.data_->category;
  } else {
    x_code = 0;
    x_category = 0;
  }
  return compare(x_code, x_category);
}

int error::compare(uint8_t x, uint8_t y) const noexcept {
  uint8_t mx;
  uint8_t my;
  if (data_ != nullptr) {
    mx = data_->code;
    my = data_->category;
  } else {
    mx = 0;
    my = 0;
  }
  // all errors with default value are considered no error -> equal
  if (mx == x && x == 0)
    return 0;
  if (my < y)
    return -1;
  if (my > y)
    return 1;
  return static_cast<int>(mx) - x;
}

// -- modifiers --------------------------------------------------------------

message& error::context() noexcept {
  CAF_ASSERT(data_ != nullptr);
  return data_->context;
}

void error::clear() noexcept {
  if (data_ != nullptr) {
    delete data_;
    data_ = nullptr;
  }
}

// -- inspection support -----------------------------------------------------

uint8_t& error::code_ref() noexcept {
  CAF_ASSERT(data_ != nullptr);
  return data_->code;
}

uint8_t& error::category_ref() noexcept {
  CAF_ASSERT(data_ != nullptr);
  return data_->category;
}

void error::init() {
  if (data_ == nullptr)
    data_ = new data;
}

std::string to_string(const error& x) {
  if (!x)
    return "none";
  return actor_system_config::render(x);
}

} // namespace caf
