#include "gtest/gtest.h"
#include "../lib/json_validator.hpp"
#include <string>
#include <map>
#include <iostream>

extern "C" {
#include "cJSON/cJSON.h"
}

using namespace std;
using namespace json_validator;
using namespace json_adapters;
using cJSON_ptr = unique_ptr<cJSON, std::function<void(cJSON*)>>;

namespace unit_tests {
/**
 *  When using a specific adapter (eg: CJSON_adapter) we need to define
 *   some alias to avoid verbose statements like Object_validator<CJSON_adapter>
 *   on every validator usage.
 */
using Object_validator = Object_validator<CJSON_adapter>;
using Array_validator = Array_validator<CJSON_adapter>;
using Int_validator = Int_validator<CJSON_adapter>;
using String_validator = String_validator<CJSON_adapter>;
using Boolean_validator = Boolean_validator<CJSON_adapter>;

TEST(COMPLETE_FUNCTIONALITY, INT_VALIDATOR_ERRORS_MAX_VALUE) {
  const string json = "{\"anInt\": 44}";

  auto validator = Object_validator({
    {"anInt", (new Int_validator())->max_value(40)}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, INT_VALIDATOR_ERRORS_MIN_VALUE) {
  const string json = "{\"anInt\": 44}";

  auto validator = Object_validator({
    {"anInt", (new Int_validator())->min_value(50)}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, INT_VALIDATOR_SUCCESS) {
  const string json = "{\"anInt\": 44, \"anotherInt\": 5000}";

  auto validator = Object_validator({
    {"anInt", (new Int_validator())->min_value(44)->max_value(44)},
    {"anotherInt", (new Int_validator())->max_value(5000)->min_value(0)}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, STRING_VALIDATOR_ERRORS_VALID) {
  const string json = "{\"aString\": \"Valid string\"}";

  auto validator = Object_validator({
    {"aString", (new String_validator())->valid({"Not a Valid string", "Another not valid string"})},
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, STRING_VALIDATOR_ERRORS_MAX_LENGTH) {
  const string json = "{\"aString\": \"Valid string\"}";

  auto validator = Object_validator({
    {"aString", (new String_validator())->max_length(4)},
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, STRING_VALIDATOR_SUCCESS) {
  const string json = "{\"aString\": \"Valid string\"}";

  auto validator = Object_validator({
    {"aString", (new String_validator())->valid({"Valid string", "Another valid string"})},
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, ARRAY_STRING_VALIDATOR_ERRORS) {
  const string json = "{\"anArray\": [\"Value1\", \"Value2\", \"Value3\"]}";

  auto validator = Object_validator({
    {"anArray", (new Array_validator())->items((new String_validator())->valid({"Value1", "!!!Value2", "Value3"}))},
    });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, ARRAY_STRING_VALIDATOR_SUCCESS) {
  const string json = "{\"anArray\": [\"Value1\", \"Value2\", \"Value3\"]}";

  auto validator = Object_validator({
    {"anArray", (new Array_validator())->items((new String_validator())->valid({"Value1", "Value2", "Value3"}))},
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, ARRAY_STRING_VALIDATOR_EDGE_CASES) {
  const string json = "{\"anArray\": []}";

  auto validator = Object_validator({
    {"anArray", (new Array_validator())},
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, MISSING_REQUIRED_FIELD) {
  const string json = "{\"anArray\": [], \"anInt\": 44, \"aString\": \"stringvalue\"}";

  auto validator = Object_validator({
    {"anArray", (new Array_validator)},
    {"requiredField", (new Int_validator())->required(true)},
    {"anInt", (new Int_validator())->max_value(50)},
    {"aString", (new String_validator())->valid({"stringvalue", "anotherstringvalue"})}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, ARRAY_INT_VALIDATOR_ERRORS) {
  const string json = "{\"anArray\": [1, 2, 3, 4, 5]}";

  auto validator = Object_validator({
    {"anArray", (new Array_validator())->items((new Int_validator())->min_value(2)->max_value(5))},
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);

  auto validator2 = Object_validator({
    {"anArray", (new Array_validator())->items((new Int_validator)->max_value(3))}
  });

  auto result2 = validator2.validate(apt);
  EXPECT_EQ(result2->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, ARRAY_INT_VALIDATOR_SUCCESS) {
  const string json = "{\"anArray\": [1, 2, 3, 4, 5]}";

  auto validator = Object_validator({
    {"anArray", (new Array_validator())->items((new Int_validator())->min_value(1)->max_value(5))},
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, ARRAY_OF_ARRAYS_ERROR) {
  const string json = "{\"anArrayOfArrays\": [[1, 2, 3, 4], [5, 6, 7, 8, 11]]}";

  auto validator = Object_validator({
    {"anArrayOfArrays", (new Array_validator())->items((new Array_validator)->items((new Int_validator())->min_value(1)->max_value(10)))}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, ARRAY_OF_ARRAYS_SUCCESS) {
  const string json = "{\"anArrayOfArrays\": [[1, 2, 3, 4], [5, 6, 7, 8]]}";

  auto validator = Object_validator({
    {"anArrayOfArrays", (new Array_validator())->items((new Array_validator)->items((new Int_validator())->min_value(1)->max_value(10)))}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, ARRAY_OF_OBJECTS) {
  const string json = "{\"anArrayOfObjects\": [{\"anInt\": 1}, {\"anInt\": 60}]}";

  auto validator = Object_validator({
    {"anArrayOfObjects", (new Array_validator())->items((new Object_validator({
      {"anInt", (new Int_validator())->min_value(0)->max_value(60)}
    })))}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, ERROR_VALIDATING_OBJECT_AS_ARRAY) {
  const string json = "{\"anObject\": {\"anInt\": 1}}";

  auto validator = Object_validator({
    {"anObject", (new Array_validator())}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, ARRAY_WITH_INVALID_ITEMS_TYPE) {
  const string json = "{\"anArray\": [1, 2, 3, 4, null]}";

  auto validator = Object_validator({
    {"anArray", (new Array_validator())->items((new Int_validator()))}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, NESTED_OBJECTS_ERROR) {
  const string json = "{\"obj1\": {\"obj2\": {\"obj3\": 4}}}";

  auto validator = Object_validator({
    {"obj1", (new Object_validator({
      {"obj2", (new Object_validator({
        {"obj3", (new Int_validator())->min_value(40)}
      }))}
    }))}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, NESTED_OBJECTS_ARRAY_OF_ARRYS_ERROR) {
  const string json = "{\"obj1\": {\"obj2\": {\"obj3\": [[1, 2, 3, 4, -1], [5, 6, 7]]}}}";

  auto validator = Object_validator({
    {"obj1", (new Object_validator({
      {"obj2", (new Object_validator({
        {"obj3", (new Array_validator())->items((new Array_validator)->items((new Int_validator())->min_value(1)->max_value(10)))}
      }))}
    }))}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);

  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, MULTIPLE_TIME_VALIDATION_ON_OBJECT) {
  const string valid_json = "{\"anObject\": {\"anInt\": 1, \"anotherInt\": 2}}";
  const string invalid_json = "{\"anObject\": {\"anInt\": 1}}";

  auto validator = Object_validator({
    {"anObject", (new Object_validator({
      {"anInt", (new Int_validator())->required(true)},
      {"anotherInt", (new Int_validator())->required(true)}
    }))}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(valid_json.c_str()), cJSON_Delete);
  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);

  cJSON_ptr root2 = cJSON_ptr(cJSON_Parse(invalid_json.c_str()), cJSON_Delete);
  CJSON_adapter apt2(root2.get());
  auto result2 = validator.validate(apt2);
  EXPECT_EQ(result2->success(), false);

  cJSON_ptr root3 = cJSON_ptr(cJSON_Parse(valid_json.c_str()), cJSON_Delete);
  CJSON_adapter apt3(root3.get());
  auto result3 = validator.validate(apt3);
  EXPECT_EQ(result3->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, MULTIPLE_TIME_VALIDATION_ON_ARRAY) {
  const string valid_json = "[{\"anObject\": {\"anInt\": 1, \"anotherInt\": 2}}]";
  const string invalid_json = "[{\"anObject\": {\"anInt\": 1}}]";

  auto validator = Array_validator();
  validator.items(new Object_validator({
    {"anObject", (new Object_validator({
      {"anInt", (new Int_validator())->required(true)},
      {"anotherInt", (new Int_validator())->required(true)}
    }))}
  }));

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(valid_json.c_str()), cJSON_Delete);
  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);

  cJSON_ptr root2 = cJSON_ptr(cJSON_Parse(invalid_json.c_str()), cJSON_Delete);
  CJSON_adapter apt2(root2.get());
  auto result2 = validator.validate(apt2);
  EXPECT_EQ(result2->success(), false);

  cJSON_ptr root3 = cJSON_ptr(cJSON_Parse(valid_json.c_str()), cJSON_Delete);
  CJSON_adapter apt3(root3.get());
  auto result3 = validator.validate(apt3);
  EXPECT_EQ(result3->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, ARRAY_MIN_MAX) {
  const string json = "{\"anArray\": [\"Value1\", \"Value2\", \"Value3\"]}";

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);
  CJSON_adapter apt(root.get());

  // Min success
  auto validator = Object_validator({
    {"anArray", (new Array_validator())->items((new String_validator()))->min(0)},
  });

  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);

  // Min error
  auto validator2 = Object_validator({
    {"anArray", (new Array_validator())->items((new String_validator()))->min(4)},
  });
  
  auto result2 = validator2.validate(apt);
  EXPECT_EQ(result2->success(), false);

  // max success
  auto validator3 = Object_validator({
    {"anArray", (new Array_validator())->items((new String_validator()))->max(4)},
  });
  
  auto result3 = validator3.validate(apt);
  EXPECT_EQ(result3->success(), true);

  // max error
  auto validator4 = Object_validator({
    {"anArray", (new Array_validator())->items((new String_validator()))->max(2)},
  });
  
  auto result4 = validator4.validate(apt);
  EXPECT_EQ(result4->success(), false);

  // Max and min
  auto validator5 = Object_validator({
    {"anArray", (new Array_validator())->items((new String_validator()))->min(3)->max(3)},
  });
  
  auto result5 = validator5.validate(apt);
  EXPECT_EQ(result5->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, OBJECT_FORBIDDEN_KEYS) {
  const string json = "{\"anObject\": {\"anInt\": 1, \"anotherInt\": 2, \"forbiddenKey\": 123}}";

  auto internal_validator = new Object_validator({
    {"anInt", (new Int_validator())->required(true)},
    {"anotherInt", (new Int_validator())->required(true)}
  });

  internal_validator->forbidden_keys({"forbiddenKey"});

  auto validator = Object_validator({
    {"anObject", internal_validator}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);
  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), false);
}

TEST(COMPLETE_FUNCTIONALITY, INT_DEFAULT_VALUE) {
  const string json = "{\"anObject\": {\"anotherInt\": 2, \"forbiddenKey\": 123}}";

  auto validator = Object_validator({
    {"anObject", new Object_validator({
      {"anInt", (new Int_validator())->default_value(66)},
      {"anotherInt", (new Int_validator())->required(true)}
    })}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);
  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);

  // From here, json MUST have anInt member with value 66
  CJSON_adapter apt2(root.get());

  auto validator2 = Object_validator({
    {"anObject", new Object_validator({
      {"anInt", (new Int_validator())->max_value(66)->min_value(66)->required(true)},
      {"anotherInt", (new Int_validator())->required(true)}
    })}
  });

  auto result2 = validator2.validate(apt);
  EXPECT_EQ(result2->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, INT_DEFAULT_VALUE_ALREADY_PRESENT) {
  const string json = "{\"anObject\": {\"anInt\": 80, \"anotherInt\": 2, \"forbiddenKey\": 123}}";

  auto validator = Object_validator({
    {"anObject", new Object_validator({
      {"anInt", (new Int_validator())->default_value(66)},
      {"anotherInt", (new Int_validator())->required(true)}
    })}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);
  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);

  // anInt MUST be 80
  CJSON_adapter apt2(root.get());

  auto validator2 = Object_validator({
    {"anObject", new Object_validator({
      {"anInt", (new Int_validator())->max_value(80)->min_value(80)->required(true)},
      {"anotherInt", (new Int_validator())->required(true)}
    })}
  });

  auto result2 = validator2.validate(apt2);
  EXPECT_EQ(result2->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, BOOLEAN_DEFAULT_VALUE) {
  const string json = "{\"anObject\": {\"anInt\": 33}}";

  auto validator = Object_validator({
    {"anObject", new Object_validator({
      {"aBool", (new Boolean_validator())->default_value(true)}
    })}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);
  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);

  // From here, json MUST have aBool member with value true
  CJSON_adapter apt2(root.get());

  auto validator2 = Object_validator({
    {"anObject", new Object_validator({
      {"aBool", (new Boolean_validator())->is(true)->required(true)}
    })}
  });

  auto result2 = validator2.validate(apt2);
  EXPECT_EQ(result2->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, BOOLEAN_DEFAULT_VALUE_ALREADY_PRESENT) {
  const string json = "{\"anObject\": {\"aBool\": false, \"anInt\": 33}}";

  auto validator = Object_validator({
    {"anObject", new Object_validator({
      {"aBool", (new Boolean_validator())->default_value(true)}
    })}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);
  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);

  // From here, json MUST have aBool member with value true
  CJSON_adapter apt2(root.get());

  auto validator2 = Object_validator({
    {"anObject", new Object_validator({
      {"aBool", (new Boolean_validator())->is(false)->required(true)}
    })}
  });

  auto result2 = validator2.validate(apt2);
  EXPECT_EQ(result2->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, STRING_DEFAULT_VALUE) {
  const string json = "{\"anObject\": {\"anInt\": 33}}";

  auto validator = Object_validator({
    {"anObject", new Object_validator({
      {"aString", (new String_validator())->default_value("a simple string")}
    })}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);
  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);

  // From here, json MUST have a string member with value "a simple string"
  CJSON_adapter apt2(root.get());

  auto validator2 = Object_validator({
    {"anObject", new Object_validator({
       {"aString", (new String_validator())->valid({"a simple string"})}
    })}
  });

  auto result2 = validator2.validate(apt2);
  EXPECT_EQ(result2->success(), true);
}

TEST(COMPLETE_FUNCTIONALITY, STRING_DEFAULT_VALUE_ALREADY_PRESENT) {
  const string json = "{\"anObject\": {\"aString\": \"a simple string\", \"anInt\": 33}}";

  auto validator = Object_validator({
    {"anObject", new Object_validator({
      {"aString", (new String_validator())->default_value("another simple string")}
    })}
  });

  cJSON_ptr root = cJSON_ptr(cJSON_Parse(json.c_str()), cJSON_Delete);
  CJSON_adapter apt(root.get());
  auto result = validator.validate(apt);
  EXPECT_EQ(result->success(), true);

  CJSON_adapter apt2(root.get());

  auto validator2 = Object_validator({
    {"anObject", new Object_validator({
       {"aString", (new String_validator())->valid({"a simple string"})}
    })}
  });

  auto result2 = validator2.validate(apt2);
  EXPECT_EQ(result2->success(), true);
}


}
