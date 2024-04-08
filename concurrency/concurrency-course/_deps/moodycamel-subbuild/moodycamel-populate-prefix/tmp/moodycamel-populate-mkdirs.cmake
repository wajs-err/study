# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/workspace/concurrency-course/_deps/moodycamel-src"
  "/workspace/concurrency-course/_deps/moodycamel-build"
  "/workspace/concurrency-course/_deps/moodycamel-subbuild/moodycamel-populate-prefix"
  "/workspace/concurrency-course/_deps/moodycamel-subbuild/moodycamel-populate-prefix/tmp"
  "/workspace/concurrency-course/_deps/moodycamel-subbuild/moodycamel-populate-prefix/src/moodycamel-populate-stamp"
  "/workspace/concurrency-course/_deps/moodycamel-subbuild/moodycamel-populate-prefix/src"
  "/workspace/concurrency-course/_deps/moodycamel-subbuild/moodycamel-populate-prefix/src/moodycamel-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/workspace/concurrency-course/_deps/moodycamel-subbuild/moodycamel-populate-prefix/src/moodycamel-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/workspace/concurrency-course/_deps/moodycamel-subbuild/moodycamel-populate-prefix/src/moodycamel-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
