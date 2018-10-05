#ifndef CJSON_VALIDATOR_VALIDATION_RESULT_HPP
#define CJSON_VALIDATOR_VALIDATION_RESULT_HPP
#include <string>
#include <vector>
#include <sstream>

namespace json_validator {
using std::string;
using std::vector;
using std::stringstream;

/**
 * Simple wrapper to validation errors
 */
class Validation_result {
  protected:
  bool _success;
  vector<string> _error_stack;

  public:
  Validation_result(bool success, const string& error)
      : _success(success) {
    this->_error_stack.push_back(error);
  }
  explicit Validation_result(bool success) : _success(success) { }
  Validation_result() : _success(true) { }

  virtual ~Validation_result() = default;

  virtual void set_error(const string& error) {
    this->_success = false;
    this->_error_stack.push_back(error);
  }

  virtual void reset() {
    _success = true;
    _error_stack.clear();
  }

  virtual string message() {
    stringstream ss;
    ss << "json";
    for (auto reverse_it = this->_error_stack.rbegin(); reverse_it !=
        this->_error_stack.rend();) {
      auto current_symbol = *reverse_it;

      //  If next symbol is not the last, add '[]' to show nested objects/arrays
      ++reverse_it;
      if (reverse_it != this->_error_stack.rend()) {
        ss << "[" << current_symbol << "]";
      } else {  //  Next symbol will be the last so add ':'
        ss << ": " << current_symbol;
      }
    }

    return "[ERROR] " + ss.str();
  }

  virtual bool success() {
    return this->_success;
  }
};

}  // namespace json_validator

#endif
