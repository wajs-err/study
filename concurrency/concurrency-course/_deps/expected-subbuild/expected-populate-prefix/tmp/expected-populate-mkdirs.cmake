# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/workspace/concurrency-course/_deps/expected-src"
  "/workspace/concurrency-course/_deps/expected-build"
  "/workspace/concurrency-course/_deps/expected-subbuild/expected-populate-prefix"
  "/workspace/concurrency-course/_deps/expected-subbuild/expected-populate-prefix/tmp"
  "/workspace/concurrency-course/_deps/expected-subbuild/expected-populate-prefix/src/expected-populate-stamp"
  "/workspace/concurrency-course/_deps/expected-subbuild/expected-populate-prefix/src"
  "/workspace/concurrency-course/_deps/expected-subbuild/expected-populate-prefix/src/expected-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/workspace/concurrency-course/_deps/expected-subbuild/expected-populate-prefix/src/expected-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/workspace/concurrency-course/_deps/expected-subbuild/expected-populate-prefix/src/expected-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
