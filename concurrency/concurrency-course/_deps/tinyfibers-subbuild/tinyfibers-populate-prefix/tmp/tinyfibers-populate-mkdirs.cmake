# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/workspace/concurrency-course/_deps/tinyfibers-src"
  "/workspace/concurrency-course/_deps/tinyfibers-build"
  "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix"
  "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/tmp"
  "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src/tinyfibers-populate-stamp"
  "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src"
  "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src/tinyfibers-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src/tinyfibers-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/workspace/concurrency-course/_deps/tinyfibers-subbuild/tinyfibers-populate-prefix/src/tinyfibers-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
