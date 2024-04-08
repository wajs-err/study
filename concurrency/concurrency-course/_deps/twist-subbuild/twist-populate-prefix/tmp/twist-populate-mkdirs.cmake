# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/workspace/concurrency-course/_deps/twist-src"
  "/workspace/concurrency-course/_deps/twist-build"
  "/workspace/concurrency-course/_deps/twist-subbuild/twist-populate-prefix"
  "/workspace/concurrency-course/_deps/twist-subbuild/twist-populate-prefix/tmp"
  "/workspace/concurrency-course/_deps/twist-subbuild/twist-populate-prefix/src/twist-populate-stamp"
  "/workspace/concurrency-course/_deps/twist-subbuild/twist-populate-prefix/src"
  "/workspace/concurrency-course/_deps/twist-subbuild/twist-populate-prefix/src/twist-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/workspace/concurrency-course/_deps/twist-subbuild/twist-populate-prefix/src/twist-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/workspace/concurrency-course/_deps/twist-subbuild/twist-populate-prefix/src/twist-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
