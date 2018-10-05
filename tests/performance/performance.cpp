#include <iostream>
#include <chrono>

#include "json_validator.hpp"

using namespace std;
using namespace json_validator;
using namespace json_adapters;
using cJSON_ptr = unique_ptr<cJSON, std::function<void(cJSON*)>>;

int main() {
  using Object_validator = Object_validator<CJSON_adapter>;
  using Array_validator = Array_validator<CJSON_adapter>;
  using Int_validator = Int_validator<CJSON_adapter>;
  using String_validator = String_validator<CJSON_adapter>;
  using Boolean_validator = Boolean_validator<CJSON_adapter>;

  auto validator = Object_validator({
    {"anIntArray", (new Array_validator())->items((new Array_validator())->length(2)->items((new Int_validator())->min_value(0)))->required(true)},
    {"andIntValue", (new Int_validator())->max_value(200)->min_value(0)},
    {"anotherIntValue", (new Int_validator())->max_value(100)->min_value(0)},
    {"aString", (new String_validator())->valid({"yes", "no", "both"})->default_value("yes")},
    {"aBoolean", (new Boolean_validator())->default_value(false)},
    {"anotherBoolean", (new Boolean_validator())->default_value(false)},
    {"yetAnotherInt", (new Int_validator())->min_value(1)->max_value(1000)->default_value(12)},
    {"yetAnotherBoolean", (new Boolean_validator())->default_value(false)},
    {"yetAnotherAnotherBoolean", (new Boolean_validator())->default_value(false)},
    {"stringArray", (new Array_validator())->unique(true)->items((new String_validator())->valid({"stringArrayValidValue1", "stringArrayValidValue2", "stringArrayValidValue3"}))},
    {"anotherStringArray", (new Array_validator())->unique(true)->items((new String_validator())->valid({"impression", "click", "install", "fetch"}))},
  });

  validator.add_validator("stringArrayValidValue1", (new Object_validator({
    {"type", (new String_validator())->valid({"OR", "AND", "NOT", "ORNOT"})},
    {"dimensionValues", (new Array_validator)->items((new String_validator())->valid({"banner", "video"}))}
  })));

  int number_of_validations = 100000;

  // Valid json
  const string valid_json = "{ \"anotherIntValue\": 99, \
                       \"andIntValue\": 200, \
                       \"anIntArray\": [[66, 77], [88, 99]], \
                       \"aString\": \"no\", \
                       \"yetAnotherInt\": 100, \
                       \"yetAnotherBoolean\": false, \
                       \"yetAnotherAnotherBoolean\": true, \
                       \"stringArray\": [ \"stringArrayValidValue1\", \"stringArrayValidValue2\", \"stringArrayValidValue3\" ], \
                       \"anotherStringArray\": [\"fetch\", \"install\"], \
                       \"stringArrayValidValue1\": { \
                          \"type\": \"OR\", \
                          \"dimensionValues\": [\"banner\", \"video\"] \
                       } \
                     }";

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(valid_json.c_str()), cJSON_Delete);
  using TimeT = std::chrono::milliseconds;

  auto start1 = std::chrono::steady_clock::now();

  CJSON_adapter apt(root.get());
  for(int i = 0; i < number_of_validations; i++) {
    auto result = validator.validate(apt);
  }

  auto duration1 = std::chrono::duration_cast< TimeT> (std::chrono::steady_clock::now() - start1);
  cout << duration1.count() << " ms to validate a valid valid_json " << number_of_validations << " times" << endl;

  // Invalid json
  const string invalid_valid_json = "{ \"anotherIntValue\": 99, \
                       \"andIntValue\": 200, \
                       \"anIntArray\": [[66, 77], [88, 99]], \
                       \"aString\": \"no\", \
                       \"yetAnotherInt\": 100, \
                       \"yetAnotherBoolean\": false, \
                       \"yetAnotherAnotherBoolean\": true, \
                       \"stringArray\": [ \"stringArrayValidValue1\", \"stringArrayValidValue2\", \"stringArrayValidValue3\", \"invalidDimension\"], \
                       \"anotherStringArray\": [\"fetch\", \"install\"], \
                       \"stringArrayValidValue1\": { \
                          \"type\": \"OR\", \
                          \"dimensionValues\": [\"banner\", \"video\"] \
                       } \
                     }";

  cJSON_ptr root2 = cJSON_ptr(cJSON_Parse(invalid_valid_json.c_str()), cJSON_Delete);

  auto start2 = std::chrono::steady_clock::now();

  CJSON_adapter apt2(root2.get());
  for(int i = 0; i < number_of_validations; i++) {
    auto result = validator.validate(apt2);
  }

  auto duration2 = std::chrono::duration_cast< TimeT> (std::chrono::steady_clock::now() - start2);
  cout << duration2.count() << " ms to validate a valid valid_json " << number_of_validations << " times" << endl;

  auto result = validator.validate(apt2);
  cout << "invald json error: " << result->message() << endl;

  return 0;
}
