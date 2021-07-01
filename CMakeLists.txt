cmake_minimum_required(VERSION 3.1)
project(templates)

# Header only library target
add_library(kangsw_templates INTERFACE)
add_library(kangsw::templates ALIAS kangsw_templates)
target_include_directories(kangsw_templates INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# SETUP TESTS
aux_source_directory("tests/automated" TEMPLATES_AUTOMATED_TEST_SOURCE)

add_executable(automated_test ${TEMPLATES_AUTOMATED_TEST_SOURCE})
target_link_libraries(automated_test kangsw_templates)
target_compile_features(automated_test PUBLIC cxx_std_20)
