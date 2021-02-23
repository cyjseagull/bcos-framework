# ------------------------------------------------------------------------------
# Copyright (C) 2021 FISCO BCOS.
# SPDX-License-Identifier: Apache-2.0
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ------------------------------------------------------------------------------
# File: Coverage.cmake
# Function: Define coverage related functions
# ------------------------------------------------------------------------------
# REMOVE_FILE_PATTERN eg.: '/usr*' '${CMAKE_SOURCE_DIR}/deps**' '${CMAKE_SOURCE_DIR}/evmc*' ‘${CMAKE_SOURCE_DIR}/fisco-bcos*’
function(config_coverage REMOVE_FILE_PATTERN)
    find_program(LCOV_TOOL lcov)
    message(STATUS "lcov tool: ${LCOV_TOOL}")
    if (LCOV_TOOL)
        add_custom_target(coverage
            COMMAND ${LCOV_TOOL} -o ${CMAKE_BINARY_DIR}/coverage.info.in -c -d ${CMAKE_BINARY_DIR}/
            COMMAND ${LCOV_TOOL} -r ${CMAKE_BINARY_DIR}/coverage.info.in ${REMOVE_FILE_PATTERN} -o ${CMAKE_BINARY_DIR}/coverage.info
            COMMAND genhtml -q -o ${CMAKE_BINARY_DIR}/CodeCoverage ${CMAKE_BINARY_DIR}/coverage.info)
    else ()
        message(FATAL_ERROR "Can't find lcov tool. Please install lcov")
    endif()
endfunction()