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

# Dynamic libraries on Windows need to be copied to the output folder.
# This is required on all platforms and becomes most obvious when
# building OpenSSL 1.1.x 64-bit on GitHub Windows runners, which have
# `libssl-1_1-x64.dll` in `C:\Windows\system32`, preventing us from
# installing our binaries anywhere but in the same directory as our
# executable (see Dynamic-Link Library Search Order in Windows
# documentation).
if (WIN32)

  # We'll need the path to the directory containing the OpenSSL DLLs.
  #

  # Compute the path relative to any of the OpenSSL `.lib` files with
  # the assumption that the `lib/` and `bin/ folders are side-by-side,
  # which is what OpenSSL's `nmake install` command does for us.
  #
  # Note that it's tempting to use `OPENSSL_INCLUDE_DIR` here, but
  # that is unreliable because it usually finds
  # `C:/Strawberry/c/include` before the "official" location.
  cmake_path(GET OPENSSL_SSL_LIBRARY PARENT_PATH OPENSSL_BINARY_DIR)
  cmake_path(GET OPENSSL_BINARY_DIR PARENT_PATH OPENSSL_BINARY_DIR)
  set(OPENSSL_BINARY_DIR "${OPENSSL_BINARY_DIR}/bin")
  message("OPENSSL_BINARY_DIR: ${OPENSSL_BINARY_DIR}")

  # Search for all DLL files in the OpenSSL binaries.  Normally, this
  # list will contain two files: `libcrypto*.dll` and `libssl*.dll`.
  # Note that the naming conventions change from OpenSSL 1.1.x to 3.x
  # and OpenSSL is free to change how many DLLs they publish, so we're
  # better off globbing for these files.
  file(GLOB OPENSSL_DYNAMIC_LIBRARIES "${OPENSSL_BINARY_DIR}/*.dll")
  message("OPENSSL_DYNAMIC_LIBRARIES: ${OPENSSL_DYNAMIC_LIBRARIES}")
endif()

# Create the C++ executable that will embed Python.
add_executable(TestOpenSSL main.cpp)

# Link against CPython.
target_link_libraries(
  TestOpenSSL
  ${OPENSSL_LIBRARIES}
)
if (WIN32)
  add_custom_command(
    TARGET TestOpenSSL POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy_if_different
      ${OPENSSL_DYNAMIC_LIBRARIES}
      "$<TARGET_FILE_DIR:TestOpenSSL>"
  )
endif()

# Add simple test to confirm that the program works.
enable_testing()
add_test(
  NAME PrintOpenSSLVersion
  COMMAND TestOpenSSL
)
set_tests_properties(PrintOpenSSLVersion PROPERTIES
  PASS_REGULAR_EXPRESSION "${EXPECTED_OPENSSL_VERSION}"
)
