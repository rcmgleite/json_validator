#ifndef CJSON_VALIDATOR_STRING_VALIDATOR_HPP
#define CJSON_VALIDATOR_STRING_VALIDATOR_HPP
#include <set>
#include <string>
#include <limits>
#include <utility>

#include "validator.hpp"
#include "json_adapters/adapter.hpp"

namespace json_validator {
using std::set;
using std::string;

using String_validator_possible_values_t = set<string>;

/**
 * String validator is used to validate json keys that contains strings as
 * value.
 *
 * Currently, this validator verifies:
 *  1) possible values for the string
 *  2) max length of the string
 */
template<typename AdapterType>
class String_validator : public Validator<AdapterType> {
  public:
  using JSON_token = json_adapters::JSON_adapter<AdapterType>;

  private:
  String_validator_possible_values_t _possible_values;

  protected:
  Validation_result_ptr validate_type(const JSON_token& token,
                                      Validation_result_ptr result) {
    result->reset();

    if (!token.is_string()) {
      result->set_error("must be a string");
    }

    return result;
  }

  Validation_result_ptr validate_max_length(const JSON_token& token,
                                            size_t max_length,
                                            Validation_result_ptr result) {
    result->reset();

    auto token_value = token.get_string();
    if (token_value.size() > max_length) {
      string error_message;
      error_message += "has length " + std::to_string(token_value.size());
      error_message += " but max_length is " + std::to_string(max_length);

      result->set_error(error_message);
    }

    return result;
  }

  Validation_result_ptr validate_possible_values(const JSON_token& token,
                   const String_validator_possible_values_t& possible_values,
                                                 Validation_result_ptr result) {
    result->reset();

    auto token_value = token.get_string();
    auto it = possible_values.find(token_value);
    if (it == possible_values.end()) {
      string error_message = "Value \"" + string(token_value);
      error_message += "\" not allowed";

      result->set_error(error_message);
    }

    return result;
  }

  void set_default_value(const JSON_token& json, const string& key) override {
    this->_set_default_value(json, key, _default_value);
  }

  public:
  String_validator() {
    this->_valdations.push_back([this] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_type(token, std::move(result));
    });
  }

  String_validator* valid(const String_validator_possible_values_t&
  possible_values) {
    this->_possible_values = possible_values;
    this->_valdations.push_back([this] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_possible_values(token, this->_possible_values,
                                            std::move(result));
    });

    return this;
  }

  String_validator* max_length(size_t max_length) {
    this->_valdations.push_back([this, max_length] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_max_length(token, max_length, std::move(result));
    });
    return this;
  }

  String_validator* default_value(const string& default_value) {
    this->_has_default_value = true;
    this->_default_value = default_value;
    return this;
  }

  private:
  string _default_value;
};
}  // namespace json_validator
#endif
