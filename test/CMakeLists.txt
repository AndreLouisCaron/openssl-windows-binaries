cmake_minimum_required(VERSION 3.22)

project(TestOpenSSL CXX)

# The default OpenSSL build creates a DLL file, so linking with
# `libcrypto*.lib` and `libssl*.lib` still requires you to ship
# `libcrypto.dll` and `libssl*.dll` alongside your executable.  However,
# OpenSSL also allows building a fully static library instead.
#
# On Linux systems, I'd recommend the shared library option because
# it allows you to ship security fixes to all OpenSSL-based applications
# that are installed without recompiling any of them.  On Windows,
# this central `libssl.dll` doesn't exist and this workflow is mostly
# broken anyways, so might as well avoid the hassle of bundling the DLL.
option(OPENSSL_DLL "Link with OpenSSL dynamically" ON)

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

# Find OpenSSL.
find_package(OpenSSL REQUIRED)
message("OpenSSL version: \"${OPENSSL_VERSION}\"")
message("OpenSSL include directory: ${OPENSSL_INCLUDE_DIR}")

# When linking against static OpenSSL, you also need to link with
# some Win32 DLLs which expose symbols used by OpenSSL.
if(WIN32 AND (NOT OPENSSL_DLL))
  list(PREPEND
    OPENSSL_LIBRARIES
    crypt32
    ws2_32
  )
endif()
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

# Dynamic libraries on Windows need to be copied to the output folder.
# This is required on all platforms and becomes most obvious when
# building OpenSSL 1.1.x 64-bit on GitHub Windows runners, which have
# `libssl-1_1-x64.dll` in `C:\Windows\system32`, preventing us from
# installing our binaries anywhere but in the same directory as our
# executable (see Dynamic-Link Library Search Order in Windows
# documentation).
if(WIN32 AND OPENSSL_DLL)

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

# Allow #include <openssl/*.h>
target_include_directories(TestOpenSSL
  PRIVATE
    ${OPENSSL_INCLUDE_DIR}
)

# Link against OpenSSL.
target_link_libraries(
  TestOpenSSL
  ${OPENSSL_LIBRARIES}
)

# When linking against dynamc OpenSSL, you also need to bundle the
# OpenSSL dynamic/shared libraries alongside your executable or it
# will crash on boot due to the missing DLLs.
if(WIN32 AND OPENSSL_DLL)
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
