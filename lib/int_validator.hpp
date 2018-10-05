#ifndef CJSON_VALIDATOR_INT_VALIDATOR_HPP
#define CJSON_VALIDATOR_INT_VALIDATOR_HPP

#include <sstream>
#include <limits>
#include <string>
#include <vector>
#include <utility>

#include "validator.hpp"
#include "json_adapters/adapter.hpp"

namespace json_validator {
using std::stringstream;

/**
 * Int validator is used to validate json keys that contains ints as
 * value.
 *
 * Currently, this validator verifies:
 *  1) max_value
 *  2) min_value
 */
template<typename AdapterType>
class Int_validator : public Validator<AdapterType> {
  public:
  using JSON_token = json_adapters::JSON_adapter<AdapterType>;

  protected:
  Validation_result_ptr validate_type(const JSON_token& token,
                                      Validation_result_ptr result) {
    result->reset();

    if (!token.is_number()) {
      result->set_error("should be an int");
    }

    return result;
  }

  Validation_result_ptr validate_max_value(const JSON_token& token,
                                           int max_value,
                                           Validation_result_ptr result) {
    result->reset();

    auto token_value = token.get_integer();
    if (token_value > max_value) {
      string error_message = "max_value = " +
          std::to_string(max_value) + " received = " +
          std::to_string(token_value);

      result->set_error(error_message);
    }

    return result;
  }

  Validation_result_ptr validate_min_value(const JSON_token& token,
                                           int min_value,
                                           Validation_result_ptr result) {
    result->reset();

    auto token_value = token.get_integer();
    if (token_value < min_value) {
      string error_message = "min_value = " +
          std::to_string(min_value) + " received = " +
          std::to_string(token_value);

      result->set_error(error_message);
    }

    return result;
  }

  void set_default_value(const JSON_token& json, const string& key) override {
    this->_set_default_value(json, key, _default_value);
  }

  public:
  Int_validator() {
    this->_valdations.push_back([this] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_type(token, std::move(result));
    });
  }

  Int_validator* max_value(int max_value) {
    this->_valdations.push_back([this, max_value] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_max_value(token, max_value, std::move(result));
    });

    return this;
  }

  Int_validator* min_value(int min_value) {
    this->_valdations.push_back([this, min_value] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_min_value(token, min_value, std::move(result));
    });

    return this;
  }

  Int_validator* default_value(int default_value) {
    this->_has_default_value = true;
    this->_default_value = default_value;
    return this;
  }

  private:
    int _default_value;
};
}  // namespace json_validator
#endif
