#ifndef CJSON_VALIDATOR_JSON_ADAPTERS_CJSON_ADAPTER_HPP
#define CJSON_VALIDATOR_JSON_ADAPTERS_CJSON_ADAPTER_HPP

#include <stdexcept>
#include <string>

#include "adapter.hpp"
#include "cJSON/cJSON.h"

namespace json_validator {
namespace json_adapters {

/**
 *  Non-const iterators impl
 */
class CJSON_object_iterator {
  public:
  explicit CJSON_object_iterator(cJSON* ptr) : _ptr(ptr) { }

  /**
   * @brief pre increment operator
   *
   *  Pre-increment and pre-decrement operators increments or
   * decrements the value of the object and returns a reference to the result.
   */
  CJSON_object_iterator& operator++() {
    _ptr = _ptr->next;
    return *this;
  }

  /**
   * @brief Post-increment
   *
   *   Creates a copy of the object,
   * increments or decrements the value of the object and returns the
   * copy from before the increment or decrement.
   *
   */
  CJSON_object_iterator operator++(int) {
    CJSON_object_iterator cpy(*this);
    ++*this;
    return cpy;
  }

  cJSON& operator*() {
    return *_ptr;
  }

  cJSON* operator->() {
    return _ptr;
  }

  bool operator==(const CJSON_object_iterator& rhs) {
    return _ptr == rhs._ptr;
  }

  bool operator!=(const CJSON_object_iterator& rhs) {
    return _ptr != rhs._ptr;
  }

  string get_name() {
    return _ptr->string;
  }

  private:
  cJSON* _ptr;
};

class CJSON_array_iterator {
  public:
  explicit CJSON_array_iterator(cJSON* ptr) : _ptr(ptr) { }

  CJSON_array_iterator& operator++() {
    _ptr = _ptr->next;
    return *this;
  }

  CJSON_array_iterator operator++(int) {
    CJSON_array_iterator cpy(*this);
    ++*this;
    return cpy;
  }

  cJSON& operator*() {
    return *_ptr;
  }

  cJSON* operator->() {
    return _ptr;
  }

  bool operator==(const CJSON_array_iterator& rhs) {
    return _ptr == rhs._ptr;
  }

  bool operator!=(const CJSON_array_iterator& rhs) {
    return _ptr != rhs._ptr;
  }

  string get_name() {
    return _ptr->string;
  }

  private:
  cJSON* _ptr;
};

/**
 *  Forward declaration of CJSON_adapter to be used in traits
 */
class CJSON_adapter;

/**
 *  Type traits for CJSON_adapter to declare the iterator types
 */
template<>
class Adapter_traits<CJSON_adapter> {
  public:
  typedef CJSON_object_iterator object_iterator;
  typedef CJSON_array_iterator array_iterator;
};

class CJSON_adapter : public JSON_adapter<CJSON_adapter> {
  public:
  typedef Adapter_traits<CJSON_adapter>::object_iterator object_iterator;
  typedef Adapter_traits<CJSON_adapter>::array_iterator array_iterator;

  explicit CJSON_adapter(cJSON* json) : _json(json) {
    if (_json == nullptr) {
      throw std::invalid_argument("CJSON_adapter: received a null cJSON*");
    }
  }
  explicit CJSON_adapter(object_iterator itr) : _json(itr.operator ->()) { }
  explicit CJSON_adapter(array_iterator itr) : _json(itr.operator ->()) { }

  /**
   *  Type verifications
   */
  bool is_object() const override {
    return (_json->type == cJSON_Object ||
            _json->type == (cJSON_IsReference | cJSON_Object));
  }

  bool is_array() const override {
    return (_json->type == cJSON_Array ||
             _json->type == (cJSON_IsReference | cJSON_Array));
  }

  bool is_boolean() const override {
    return (_json->type == cJSON_False || _json->type == cJSON_True);
  }

  bool is_string() const override {
    return _json->type == cJSON_String;
  }

  bool is_integer() const override {
    return _json->type == cJSON_Number;
  }

  bool is_double() const override {
    return _json->type == cJSON_Number;
  }

  /**
   *  Getters
   */
  string get_name() const override {
    return _json->string;
  }

  int get_integer() const override {
    return _json->valueint;
  }

  double get_double() const override {
    return _json->valuedouble;
  }

  bool get_boolean() const override {
    if (_json->type == cJSON_True) {
      return true;
    }

    return false;
  }

  string get_string() const override {
    return _json->valuestring;
  }

  int get_array_size() const override {
    return cJSON_GetArraySize(_json);
  }

  /**
   *  Setters
   */
  void add_field(const string& field_name, int value) {
    cJSON_AddNumberToObject(_json, field_name.c_str(), value);
  }

  void add_field(const string& field_name, const string& value) {
    cJSON_AddStringToObject(_json, field_name.c_str(), value.c_str());
  }

  void add_field(const string& field_name, bool value) {
    cJSON_AddBoolToObject(_json, field_name.c_str(), value);
  }

  /**
   *  Iterators
   */
  object_iterator object_begin() {
    if (!is_object()) {
      std::invalid_argument("Trying to get object iterator from something"
                            " not an object");
    }

    return CJSON_object_iterator(_json->child);
  }

  object_iterator object_end() {
    return CJSON_object_iterator(nullptr);
  }

  array_iterator array_begin() {
    if (!is_array()) {
      std::invalid_argument("Trying to get array iterator from something"
                            " not an array");
    }

    return CJSON_array_iterator(_json->child);
  }

  array_iterator array_end() {
    return CJSON_array_iterator(nullptr);
  }

  private:
  cJSON* _json;
};

/**
 * @brief Factory method for json_adapters
 * @param json
 */
CJSON_adapter json_adapter_factory(cJSON* json) {
  return CJSON_adapter(json);
}

using object_iterator_t =
  typename Adapter_traits<CJSON_adapter>::object_iterator;
CJSON_adapter json_adapter_factory(object_iterator_t itr) {
  return CJSON_adapter(itr);
}

using array_iterator_t = typename Adapter_traits<CJSON_adapter>::array_iterator;
CJSON_adapter json_adapter_factory(array_iterator_t itr) {
  return CJSON_adapter(itr);
}


}  // namespace json_adapters
}  // namespace json_validator

#endif
