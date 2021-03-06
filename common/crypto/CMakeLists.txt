# Copyright 2020 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0

CMAKE_MINIMUM_REQUIRED(VERSION 3.2 FATAL_ERROR)

INCLUDE(../../cmake/ConfigSGX.cmake)

INCLUDE(CMakeVariables.txt)

SET(PDO_CRYPTO_DIR "pdo/common/crypto")
SET(PDO_CRYPTO_TEST_DIR "${PDO_CRYPTO_DIR}/../tests/crypto")

FILE(GLOB PROJECT_HEADERS
    "${PDO_CRYPTO_DIR}/../*.h"
    "attestation-api/evidence/*.h"
    )
FILE(GLOB PROJECT_SOURCES
    "${PDO_CRYPTO_DIR}/../c11_support.cpp"
    "${PDO_CRYPTO_DIR}/../hex_string.cpp"
    "${PDO_CRYPTO_DIR}/../log.cpp"
    "pdo-types.cpp"
    "$ENV{FPC_PATH}/common/base64/base64.cpp"
    "$ENV{FPC_PATH}/common/json/parson.c"
    "attestation-api/evidence/*.cpp"
    )

FILE(GLOB PROJECT_OCALLS
    "attestation-api/ocalls/*.c"
    )

FILE(GLOB PROJECT_TRUSTED_SOURCES
    "attestation-api/attestation/*.cpp"
    )

FILE(GLOB PROJECT_TRUSTED_HEADERS
    "attestation-api/attestation/*.h"
    )

#set debug flags for PDO based on FPC flags
#(COMMON_CXX_FLAGS is inherited, and used, by the pdo crypto tests)
if("$ENV{PDO_DEBUG_BUILD}" STREQUAL "1")
    SET(COMMON_CXX_FLAGS "-Og" "-g" "-DPDO_DEBUG_BUILD=1")
else()
    SET(COMMON_CXX_FLAGS "-O2" "-DPDO_DEBUG_BUILD=0")
endif()


###################################################################################################
# Untrusted crypto adapt library
###################################################################################################
ADD_LIBRARY(${U_CRYPTO_ADAPT_LIB_NAME} STATIC ${PROJECT_HEADERS} ${PROJECT_SOURCES} ${PROJECT_OCALLS})
TARGET_INCLUDE_DIRECTORIES(${U_CRYPTO_ADAPT_LIB_NAME} PRIVATE "$ENV{FPC_PATH}/common")
TARGET_INCLUDE_DIRECTORIES(${U_CRYPTO_ADAPT_LIB_NAME} PRIVATE "${PDO_CRYPTO_DIR}/..")
TARGET_INCLUDE_DIRECTORIES(${U_CRYPTO_ADAPT_LIB_NAME} PRIVATE "${PDO_CRYPTO_DIR}")
TARGET_INCLUDE_DIRECTORIES(${U_CRYPTO_ADAPT_LIB_NAME} PRIVATE "$ENV{FPC_PATH}/common/base64")
TARGET_INCLUDE_DIRECTORIES(${U_CRYPTO_ADAPT_LIB_NAME} PRIVATE "$ENV{FPC_PATH}/common/json")
TARGET_INCLUDE_DIRECTORIES(${U_CRYPTO_ADAPT_LIB_NAME} PRIVATE "$ENV{FPC_PATH}/common/crypto")
TARGET_INCLUDE_DIRECTORIES(${U_CRYPTO_ADAPT_LIB_NAME} PRIVATE "${LOGGING_PATH}/untrusted")
TARGET_INCLUDE_DIRECTORIES(${U_CRYPTO_ADAPT_LIB_NAME} PRIVATE "$ENV{FPC_PATH}/common/crypto/attestation-api/attestation")
TARGET_INCLUDE_DIRECTORIES(${U_CRYPTO_ADAPT_LIB_NAME} PUBLIC "$ENV{SGX_SDK}/include")
TARGET_COMPILE_OPTIONS(${U_CRYPTO_ADAPT_LIB_NAME} PRIVATE ${COMMON_CXX_FLAGS} "-D_UNTRUSTED_=1")

###################################################################################################
# Trusted crypto adapt library
###################################################################################################
ADD_LIBRARY(${T_CRYPTO_ADAPT_LIB_NAME} STATIC
    ${PROJECT_HEADERS} ${PROJECT_TRUSTED_HEADERS} ${PROJECT_SOURCES} ${PROJECT_TRUSTED_SOURCES})
TARGET_INCLUDE_DIRECTORIES(${T_CRYPTO_ADAPT_LIB_NAME} PRIVATE "$ENV{SGX_SSL}/include")
TARGET_INCLUDE_DIRECTORIES(${T_CRYPTO_ADAPT_LIB_NAME} PRIVATE "$ENV{FPC_PATH}/common")
TARGET_INCLUDE_DIRECTORIES(${T_CRYPTO_ADAPT_LIB_NAME} PRIVATE "$ENV{FPC_PATH}/common/crypto")
TARGET_INCLUDE_DIRECTORIES(${T_CRYPTO_ADAPT_LIB_NAME} PRIVATE "$ENV{FPC_PATH}/common/logging/trusted")
TARGET_INCLUDE_DIRECTORIES(${T_CRYPTO_ADAPT_LIB_NAME} PRIVATE "${PDO_CRYPTO_DIR}/..")
TARGET_INCLUDE_DIRECTORIES(${T_CRYPTO_ADAPT_LIB_NAME} PRIVATE "${PDO_CRYPTO_DIR}")
TARGET_INCLUDE_DIRECTORIES(${T_CRYPTO_ADAPT_LIB_NAME} PRIVATE "$ENV{FPC_PATH}/common/base64")
TARGET_INCLUDE_DIRECTORIES(${T_CRYPTO_ADAPT_LIB_NAME} PRIVATE "$ENV{FPC_PATH}/common/crypto/attestation-api/attestation")
TARGET_INCLUDE_DIRECTORIES(${T_CRYPTO_ADAPT_LIB_NAME} PRIVATE "$ENV{FPC_PATH}/common/json")
TARGET_INCLUDE_DIRECTORIES(${T_CRYPTO_ADAPT_LIB_NAME} PUBLIC "$ENV{SGX_SDK}/include")
TARGET_COMPILE_OPTIONS(${T_CRYPTO_ADAPT_LIB_NAME} PRIVATE ${COMMON_CXX_FLAGS} -nostdinc++)
TARGET_INCLUDE_DIRECTORIES(${T_CRYPTO_ADAPT_LIB_NAME} PUBLIC "${SGX_SDK}/include/libcxx")
TARGET_INCLUDE_DIRECTORIES(${T_CRYPTO_ADAPT_LIB_NAME} PUBLIC "${SGX_SDK}/include/tlibc")
TARGET_COMPILE_OPTIONS(${T_CRYPTO_ADAPT_LIB_NAME} PRIVATE "-DENCLAVE_CODE")

###################################################################################################
# PDO Crypto library
###################################################################################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
ADD_SUBDIRECTORY (${PDO_CRYPTO_DIR})

###################################################################################################
# PDO Crypto Tests
###################################################################################################
# cmakevariable needed to load function definition
INCLUDE(${PDO_CRYPTO_DIR}/../CMakeVariables.txt)
# pdo top dir needed for includes
SET(PDO_TOP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/pdo")
# lib names needed at link time
SET(TRUSTED_LIB_NAME ${T_CRYPTO_ADAPT_LIB_NAME})
SET(UNTRUSTED_LIB_NAME ${U_CRYPTO_ADAPT_LIB_NAME})
# pkg_check_modules needed to set OPENSSL_LDFLAGS
find_package(PkgConfig REQUIRED)
pkg_check_modules (OPENSSL REQUIRED openssl>=1.1.0g)
# add pdo crypto tests
ADD_SUBDIRECTORY (${PDO_CRYPTO_TEST_DIR})

###################################################################################################
# Local Tests
###################################################################################################
ADD_SUBDIRECTORY (attestation-api/test)
