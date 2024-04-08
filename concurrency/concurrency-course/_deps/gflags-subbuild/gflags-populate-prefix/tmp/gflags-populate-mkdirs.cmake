# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/workspace/concurrency-course/_deps/gflags-src"
  "/workspace/concurrency-course/_deps/gflags-build"
  "/workspace/concurrency-course/_deps/gflags-subbuild/gflags-populate-prefix"
  "/workspace/concurrency-course/_deps/gflags-subbuild/gflags-populate-prefix/tmp"
  "/workspace/concurrency-course/_deps/gflags-subbuild/gflags-populate-prefix/src/gflags-populate-stamp"
  "/workspace/concurrency-course/_deps/gflags-subbuild/gflags-populate-prefix/src"
  "/workspace/concurrency-course/_deps/gflags-subbuild/gflags-populate-prefix/src/gflags-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/workspace/concurrency-course/_deps/gflags-subbuild/gflags-populate-prefix/src/gflags-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/workspace/concurrency-course/_deps/gflags-subbuild/gflags-populate-prefix/src/gflags-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
