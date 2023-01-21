cmake_minimum_required(VERSION 3.22)

project(TestOpenSSL CXX)

# When searching for 32-bit OpenSSL on a Windows machine that has
# Strawberry Perl (as recommended by the OpenSSL documentation and as
# installed on GitHub runners), the built-in `FindOpenSSL` module will
# incorrectly find the include files in `C:/Strawberry/c/include`,
# which doesn't provide the same OpenSSL version as the one we're
# looking for.  For this reason, we will check tests against the
# EXPECTED OpenSSL version and ignore the `OPENSSL_VERSION` variable
# provided by the `FindOpenSSL` module.
option(EXPECTED_OPENSSL_VERSION "")
message("Expected OpenSSL version: \"${EXPECTED_OPENSSL_VERSION}\"")

# Find Python.
find_package(OpenSSL REQUIRED)
message("OpenSSL version: \"${OPENSSL_VERSION}\"")
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
  PASS_REGULAR_EXPRESSION "${EXPECTED_OPENSSL_VERSION}"
)
