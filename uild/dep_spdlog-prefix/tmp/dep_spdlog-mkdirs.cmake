# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/phn24/Documents/work/cg/second_opengl_example/uild/dep_spdlog-prefix/src/dep_spdlog"
  "C:/Users/phn24/Documents/work/cg/second_opengl_example/uild/dep_spdlog-prefix/src/dep_spdlog-build"
  "C:/Users/phn24/Documents/work/cg/second_opengl_example/uild/dep_spdlog-prefix"
  "C:/Users/phn24/Documents/work/cg/second_opengl_example/uild/dep_spdlog-prefix/tmp"
  "C:/Users/phn24/Documents/work/cg/second_opengl_example/uild/dep_spdlog-prefix/src/dep_spdlog-stamp"
  "C:/Users/phn24/Documents/work/cg/second_opengl_example/uild/dep_spdlog-prefix/src"
  "C:/Users/phn24/Documents/work/cg/second_opengl_example/uild/dep_spdlog-prefix/src/dep_spdlog-stamp"
)

set(configSubDirs Debug;Release;MinSizeRel;RelWithDebInfo)
foreach(subDir IN LISTS configSubDirs)
  file(MAKE_DIRECTORY "C:/Users/phn24/Documents/work/cg/second_opengl_example/uild/dep_spdlog-prefix/src/dep_spdlog-stamp/${subDir}")
endforeach()
