cmake_minimum_required(VERSION 3.1)
project(templates)

include_directories("include")

aux_source_directory("tests/automated" TEMPLATES_AUTOMATED_TEST_SOURCE)

add_executable(automated_test ${TEMPLATES_AUTOMATED_TEST_SOURCE})
target_compile_features(automated_test PUBLIC cxx_std_20)