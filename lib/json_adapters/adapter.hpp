#ifndef CJSON_VALIDATOR_JSON_ADAPTERS_ADAPTER_HPP
#define CJSON_VALIDATOR_JSON_ADAPTERS_ADAPTER_HPP

#include <string>

namespace json_validator {
namespace json_adapters {

using std::string;

/**
 *  Traits class used by the adapter iterators
 */
template<typename AdapterType>
class Adapter_traits { };

/**
 * @brief The JSON_adapter class is the base class for all json libraries adapters.
 *
 * It provides methods to access all internal fields and also
 * an iterator interface so that the validator can loop through all
 * json fields (both from objetcs and arrays)
 *
 * @param AdapterType is passed as template so JSON_adapter
 *   can access the specific iterators on generic functions
 *
 * @see https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
 */
template <typename AdapterType>
class JSON_adapter {
  public:
  virtual ~JSON_adapter() = default;

  /**
   *  Type verifications
   */
  virtual bool is_object() const = 0;
  virtual bool is_array() const = 0;
  virtual bool is_boolean() const = 0;
  virtual bool is_string() const = 0;
  virtual bool is_integer() const = 0;
  virtual bool is_double() const = 0;
  virtual bool is_number() const {
    return is_integer() || is_double();
  }

  /**
   *  Getters
   */
  virtual string get_name() const = 0;
  virtual int get_integer() const = 0;
  virtual double get_double() const = 0;
  virtual bool get_boolean() const = 0;
  virtual string get_string() const = 0;
  virtual int get_array_size() const = 0;

  /**
   *  Setters
   */
  virtual void add_field(const string& field_name, int value) = 0;
  virtual void add_field(const string& field_name, const string& value) = 0;
  virtual void add_field(const string& field_name, bool value) = 0;

  /**
   *  Object iterator
   */
  typename Adapter_traits<AdapterType>::object_iterator object_begin() const {
    return static_cast<AdapterType*>(
             const_cast<JSON_adapter<AdapterType>*>(this))->object_begin();
  }

  typename Adapter_traits<AdapterType>::object_iterator object_end() const {
    return static_cast<AdapterType*>(
             const_cast<JSON_adapter<AdapterType>*>(this))->object_end();
  }

  /**
   *  Array Iterator
   */
  typename Adapter_traits<AdapterType>::array_iterator array_begin() const {
    return static_cast<AdapterType*>(
             const_cast<JSON_adapter<AdapterType>*>(this))->array_begin();
  }

  typename Adapter_traits<AdapterType>::array_iterator array_end() const {
    return static_cast<AdapterType*>(
             const_cast<JSON_adapter<AdapterType>*>(this))->array_end();
  }
};

}  // namespace json_adapters
}  // namespace json_validator

#endif
