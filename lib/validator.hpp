#ifndef CJSON_VALIDATOR_VALIDATOR_HPP
#define CJSON_VALIDATOR_VALIDATOR_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "validation_result.hpp"
#include "json_adapters/adapter.hpp"

namespace json_validator {

using std::unique_ptr;
using std::vector;
using Validation_result_ptr = unique_ptr<Validation_result>;

/**
 *  Forward declaration of Object_validator template class
 *  needed for friendship with Validator template class
 */
template<typename T>
class Object_validator;

/**
 * Validator base class.
 *  New validators must always extend this interface.
 */
template<typename AdapterType>
class Validator {
  public:
  using JSON_token = json_adapters::JSON_adapter<AdapterType>;
  using validation_func_t =
    std::function<Validation_result_ptr(const JSON_token& token,
                                          Validation_result_ptr result)>;

  /**
   *  Friendship needed in order to Object_validator
   *   can access protected members from Validator in their method
   *   implementations
   */
  friend class Object_validator<AdapterType>;

  private:
  bool _required;
  bool _used;

  protected:
  std::vector<validation_func_t> _valdations;
  bool _has_default_value;

  protected:
  template<typename ...Args>
  void _set_default_value(const JSON_token& json, const string& key,
                         Args&&... args) {
    // FIXME - Horible trick :'(
    const_cast<JSON_token&>(json).add_field(key, std::forward<Args>(args)...);
  }

  virtual void set_default_value(const JSON_token& json, const string& key) {
    throw(std::invalid_argument("Trying to set default value for a type "
                                "that doesn't support this operation"));
  }

  public:
  Validator() : _required(false), _used(false), _has_default_value(false) { }

  virtual ~Validator() = default;

  Validation_result_ptr validate(const JSON_token& token) {
    Validation_result_ptr result(new Validation_result());
    return validate(token, std::move(result));
  }

  Validation_result_ptr validate(const JSON_token& token,
                                 Validation_result_ptr result) {
    this->used(true);
    for (auto& validation_func : this->_valdations) {
      result = validation_func(token, std::move(result));
      if (!result->success()) {
        break;
      }
    }

    return result;
  }

  virtual Validator* required(bool required) {
    this->_required = required;
    return this;
  }

  virtual bool required() {
    return this->_required;
  }

  virtual void used(bool used) {
    this->_used = used;
  }

  virtual bool used() {
    return this->_used;
  }

  virtual bool has_default_value() {
    return _has_default_value;
  }
};
}  // namespace json_validator

#endif
