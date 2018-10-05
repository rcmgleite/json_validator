#ifndef CJSON_VALIDATOR_ARRAY_VALIDATOR_HPP
#define CJSON_VALIDATOR_ARRAY_VALIDATOR_HPP
#include <set>
#include <string>
#include <cstring>
#include <sstream>
#include <functional>
#include <stdexcept>
#include <limits>
#include <cstdint>
#include <memory>
#include <utility>

#include "json_adapters/adapter.hpp"
#include "validator.hpp"
#include "string_validator.hpp"
#include "int_validator.hpp"
#include "object_validator.hpp"

namespace json_validator {
using std::set;
using std::string;
using std::stringstream;

/**
 * Array validator is used to validate json keys that contains arrays as
 * value.
 *
 * Currently, this validator verifies:
 *  1) unique
 *  2) length
 *  3) min length
 *  4) max length
 *  5) any other validation that can be provided by an implementation of the
 *    Validator interface
 */
template<typename AdapterType>
class Array_validator : public Validator<AdapterType> {
  public:
  using JSON_token = json_adapters::JSON_adapter<AdapterType>;

  private:
  Validator<AdapterType>* _values_validator;

  protected:
  Validation_result_ptr validate_type(const JSON_token& token,
                                      Validation_result_ptr result) {
    result->reset();


    if (!token.is_array()) {
      result->set_error("must be an array");
    }

    return result;
  }

  Validation_result_ptr validate_length(const JSON_token& token,
                                        uint64_t length,
                                        Validation_result_ptr result) {
    result->reset();

    if ((uint64_t)token.get_array_size() != length) {
      result->set_error("length must be " + std::to_string(length));
    }

    return result;
  }

  Validation_result_ptr validate_min(const JSON_token& token,
                                        uint64_t length,
                                        Validation_result_ptr result) {
    result->reset();

    if ((uint64_t)token.get_array_size() < length) {
      result->set_error("length must be greater then " +
                        std::to_string(length));
    }

    return result;
  }

  Validation_result_ptr validate_max(const JSON_token& token,
                                        uint64_t length,
                                        Validation_result_ptr result) {
    result->reset();

    if ((uint64_t)token.get_array_size() > length) {
      result->set_error("length must be less then " + std::to_string(length));
    }

    return result;
  }

  Validation_result_ptr validate_valid_items(const JSON_token& token,
                                             Validator<AdapterType>* validator,
                                             Validation_result_ptr result) {
    result->reset();

    int item_index = 0;
    for (auto array_itr = token.array_begin(); array_itr != token.array_end();
         ++array_itr) {
      result = validator->validate(json_adapter_factory(array_itr),
                                   std::move(result));

      if (!result->success()) {
        result->set_error(std::to_string(item_index));
        break;
      }

      item_index++;
    }

    return result;
  }

  public:
  Array_validator() : _values_validator(nullptr) {
    this->_valdations.push_back([this] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_type(token, std::move(result));
    });
  }

  ~Array_validator() {
    delete this->_values_validator;
  }

  Array_validator* unique(bool unique) {
    this->_valdations.push_back([this] (const JSON_token& token,
                                Validation_result_ptr result) {
      return result;
    });

    return this;
  }

  Array_validator* length(uint64_t length) {
    this->_valdations.push_back([this, length] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_length(token, length, std::move(result));
    });

    return this;
  }

  Array_validator* min(uint64_t length) {
    this->_valdations.push_back([this, length] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_min(token, length, std::move(result));
    });

    return this;
  }

  Array_validator* max(uint64_t length) {
    this->_valdations.push_back([this, length] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_max(token, length, std::move(result));
    });

    return this;
  }

  Array_validator* items(Validator<AdapterType>* validator) {
    this->_values_validator = validator;
    this->_valdations.push_back([this, validator] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_valid_items(token, validator, std::move(result));
    });

    return this;
  }
};
}  // namespace json_validator
#endif
