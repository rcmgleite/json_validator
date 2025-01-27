.DEFAULT_GOAL := test
LIB_FILES := $(shell find lib -name '*.hpp')

mkdir_build:
	mkdir -p build

run_cmake: configure
	cd build && cmake .. && cd -

build_tests: 
	python2 cpplint.py --extensions=hpp $(LIB_FILES) && cd build && $(MAKE) unit_tests && cd -

run_tests:
	./build/unit_tests

run_tests_valgrind:
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./build/unit_tests

configure:
	git submodule init && git submodule update

test: mkdir_build run_cmake build_tests run_tests

test_valgrind: mkdir_build run_cmake build_tests run_tests_valgrind

performance_test: mkdir_build run_cmake 
	cd build && $(MAKE) performance_tests && ./performance_tests

clean:
	$(RM) -r build

.PHONY: mkdir_build run_cmake build_tests run_tests run_tests_valgrind test test_valgrind clean
