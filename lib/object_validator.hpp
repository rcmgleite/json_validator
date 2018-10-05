#ifndef CJSON_VALIDATOR_OBJECT_VALIDATOR_HPP
#define CJSON_VALIDATOR_OBJECT_VALIDATOR_HPP

#include <map>
#include <string>
#include <set>
#include <iostream>
#include <sstream>
#include <utility>
#include <initializer_list>

#include "validator.hpp"
#include "json_adapters/adapter.hpp"

namespace json_validator {
using std::map;
using std::string;
using std::set;
using std::cout;
using std::endl;
using std::stringstream;
using std::initializer_list;

/**
 * Array validator is used to validate json keys that contains arrays as
 * value.
 *
 * An Object_validator contains a map of keys to validators.
 * This makes it possible to validate any types inside a json object
 */

template<typename AdapterType>
class Object_validator : public Validator<AdapterType> {
  public:
  using JSON_token = json_adapters::JSON_adapter<AdapterType>;
  using map_validator_t = map<string, Validator<AdapterType>*>;

  private:
  /**
   * RAII class to manager object validator state
   *    This is needed so that the same validator can be used multiple times.
   */
  class Object_validation_state_manager {
    private:
    Object_validator* _validator;

    public:
    explicit Object_validation_state_manager(Object_validator* validator) :
        _validator(validator) { }

    ~Object_validation_state_manager() {
      for (auto it = this->_validator->_validators.begin(); it !=
          this->_validator->_validators.end(); ++it) {
        it->second->used(false);
      }
    }
  };

  map_validator_t _validators;
  set<string> _forbidden_keys;

  protected:
  Validation_result_ptr validate_type(const JSON_token& json,
                                      Validation_result_ptr result) {
    result->reset();

    if (!json.is_object()) {
      result->set_error("field must be an object");
    }

    return result;
  }

  bool is_forbidden_key(const string& key) {
    return (_forbidden_keys.size() > 0 &&
            _forbidden_keys.find(key) != _forbidden_keys.end());
  }

  Validation_result_ptr validate_object(const JSON_token& json,
                                        Validation_result_ptr result) {
    Object_validation_state_manager(this);
    result->reset();

    for (auto json_itr = json.object_begin(); json_itr != json.object_end();
         ++json_itr) {
      string current_key(json_itr.get_name());
      if (is_forbidden_key(current_key)) {
        result->set_error("'" + current_key + "' key is not allowed");
        return result;
      }

      auto it = this->_validators.find(current_key);
      if (it != this->_validators.end()) {
        result = it->second->validate(json_adapter_factory(json_itr),
                                      std::move(result));

        if (!result->success()) {
          if (current_key != "") {
            result->set_error("'" + current_key + "'");
          }

          return result;
        }
      }
    }

    // Verify required fields and set defaults if they are present
    for (auto it = this->_validators.begin(); it != this->_validators.end();
         ++it) {
      if (!it->second->used()) {
        if (it->second->has_default_value()) {
          it->second->set_default_value(json, it->first);
        } else {
          if (it->second->required()) {
            string error = it->first + " is required";
            result->set_error(error);
            break;
          }
        }
      }
    }

    return result;
  }

  public:
  explicit Object_validator(const map_validator_t& m) : _validators(m) {
    this->_valdations.push_back([this] (const JSON_token& json,
                                Validation_result_ptr result) {
      return this->validate_type(json, std::move(result));
    });

    this->_valdations.push_back([this] (const JSON_token& json,
                                Validation_result_ptr result) {
      return this->validate_object(json, std::move(result));
    });
  }

  ~Object_validator() {
    for (auto it = this->_validators.begin(); it != this->_validators.end();
         ++it) {
      delete it->second;
    }
  }

  Object_validator<AdapterType>* add_validator(const string& key,
                                            Validator<AdapterType>* validator) {
    this->_validators.insert({key, validator});
    return this;
  }

  Object_validator<AdapterType>* forbidden_keys(initializer_list<string> keys) {
    _forbidden_keys = keys;
    return this;
  }
};
}  // namespace json_validator
#endif
