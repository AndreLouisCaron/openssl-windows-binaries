cmake_minimum_required(VERSION 3.22)

project(TestOpenSSL CXX)

# Find Python.
find_package(OpenSSL REQUIRED)
message("OpenSSL version: ${OPENSSL_VERSION}")
message("OpenSSL include directory: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")
include_directories(${OPENSSL_INCLUDE_DIR})

# Create the C++ executable that will embed Python.
add_executable(TestOpenSSL main.cpp)

# Link against CPython.
target_link_libraries(
  TestOpenSSL
  ${OPENSSL_LIBRARIES}
)

# Add simple test to confirm that the program works.
enable_testing()
add_test(
  NAME PrintOpenSSLVersion
  COMMAND TestOpenSSL
)
set_tests_properties(PrintOpenSSLVersion PROPERTIES
  PASS_REGULAR_EXPRESSION "${OPENSSL_VERSION}"
)
