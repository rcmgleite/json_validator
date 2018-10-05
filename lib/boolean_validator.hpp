#ifndef CJSON_VALIDATOR_BOOLEAN_VALIDATOR_HPP
#define CJSON_VALIDATOR_BOOLEAN_VALIDATOR_HPP
#include <string>
#include <utility>

#include "validator.hpp"
#include "json_adapters/adapter.hpp"

namespace json_validator {

/**
 * Boolean validator is used to validate json keys that contains booleans as
 * value.
 *
 * Currently, this validator only verifies that the value is actually a boolean
 */
template<typename AdapterType>
class Boolean_validator : public Validator<AdapterType> {
  public:
  using JSON_token = json_adapters::JSON_adapter<AdapterType>;

  protected:
  Validation_result_ptr validate_type(const JSON_token& token,
                                      Validation_result_ptr result) {
    result->reset();
    if (!token.is_boolean()) {
      result->set_error("should be a boolean");
    }

    return result;
  }

  Validation_result_ptr validate_value(const JSON_token& token,
                                       bool value,
                                       Validation_result_ptr result) {
    result->reset();
    if (token.get_boolean() != value) {
      result->set_error("should have value: " + std::to_string(value));
    }

    return result;
  }

  void set_default_value(const JSON_token& json, const string& key) override {
    this->_set_default_value(json, key, _default_value);
  }

  public:
  Boolean_validator() {
    this->_valdations.push_back([this] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_type(token, std::move(result));
    });
  }

  Boolean_validator* default_value(bool default_value) {
    this->_has_default_value = true;
    this->_default_value = default_value;
    return this;
  }

  Boolean_validator* is(bool value) {
    this->_valdations.push_back([this, value] (const JSON_token& token,
                                Validation_result_ptr result) {
      return this->validate_value(token, value, std::move(result));
    });

    return this;
  }

  private:
  bool _default_value;
};

}  // namespace json_validator

#endif
