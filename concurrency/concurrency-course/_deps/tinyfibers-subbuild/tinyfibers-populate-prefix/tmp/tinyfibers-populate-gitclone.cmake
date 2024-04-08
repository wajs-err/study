# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

if(EXISTS "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src/tinyfibers-populate-stamp/tinyfibers-populate-gitclone-lastrun.txt" AND EXISTS "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src/tinyfibers-populate-stamp/tinyfibers-populate-gitinfo.txt" AND
  "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src/tinyfibers-populate-stamp/tinyfibers-populate-gitclone-lastrun.txt" IS_NEWER_THAN "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src/tinyfibers-populate-stamp/tinyfibers-populate-gitinfo.txt")
  message(STATUS
    "Avoiding repeated git clone, stamp file is up to date: "
    "'/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src/tinyfibers-populate-stamp/tinyfibers-populate-gitclone-lastrun.txt'"
  )
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "/workspace/concurrency-course/_deps/tinyfibers-src"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/workspace/concurrency-course/_deps/tinyfibers-src'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git" 
            clone --no-checkout --config "advice.detachedHead=false" "https://gitlab.com/Lipovsky/tinyfibers.git" "tinyfibers-src"
    WORKING_DIRECTORY "/workspace/concurrency-course/_deps"
    RESULT_VARIABLE error_code
  )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once: ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://gitlab.com/Lipovsky/tinyfibers.git'")
endif()

execute_process(
  COMMAND "/usr/bin/git" 
          checkout "f51e497872b8577b32a163f96d1ff93e47dd745e" --
  WORKING_DIRECTORY "/workspace/concurrency-course/_deps/tinyfibers-src"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'f51e497872b8577b32a163f96d1ff93e47dd745e'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/usr/bin/git" 
            submodule update --recursive --init 
    WORKING_DIRECTORY "/workspace/concurrency-course/_deps/tinyfibers-src"
    RESULT_VARIABLE error_code
  )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/workspace/concurrency-course/_deps/tinyfibers-src'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src/tinyfibers-populate-stamp/tinyfibers-populate-gitinfo.txt" "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src/tinyfibers-populate-stamp/tinyfibers-populate-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src/tinyfibers-populate-stamp/tinyfibers-populate-gitclone-lastrun.txt'")
endif()
