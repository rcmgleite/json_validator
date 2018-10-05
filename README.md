# [joi-like](https://github.com/hapijs/joi) json validator.
- [Getting started](#getting-started)
- [Usage](#usage)
- [Design principles](#design-principles)
- [Contributing](#contributing)

## Getting started
First thing to do: run the tests
```
$ make 
```

## Usage
See [tests](https://github.com/rcmgleite/json_validator/blob/master/tests/unit/complete_functionality_test.cpp).

## Design principles
1.  The API implemented was borrowed from [joi](https://github.com/hapijs/joi) (with some modifications, of course)
2.  Write code minding your colleagues who will come after you.
3.  Performance is very important. Do not overlook it.
4.  Make everything as simple as possible.
5.  Always write tests.

## Supported adapters
  - [x] [cjson](https://github.com/DaveGamble/cJSON)
  - [ ] [rapid_json](https://github.com/Tencent/rapidjson)

## TODOs
1.  Support [json-schema](http://json-schema.org)
2.  Add more adapters

## Contributing
See [Contributing](CONTRIBUTING.md)
