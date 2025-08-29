# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/landy/Desktop/selfProject/citybits/build/_deps/xxhash-src")
  file(MAKE_DIRECTORY "/Users/landy/Desktop/selfProject/citybits/build/_deps/xxhash-src")
endif()
file(MAKE_DIRECTORY
  "/Users/landy/Desktop/selfProject/citybits/build/_deps/xxhash-build"
  "/Users/landy/Desktop/selfProject/citybits/build/_deps/xxhash-subbuild/xxhash-populate-prefix"
  "/Users/landy/Desktop/selfProject/citybits/build/_deps/xxhash-subbuild/xxhash-populate-prefix/tmp"
  "/Users/landy/Desktop/selfProject/citybits/build/_deps/xxhash-subbuild/xxhash-populate-prefix/src/xxhash-populate-stamp"
  "/Users/landy/Desktop/selfProject/citybits/build/_deps/xxhash-subbuild/xxhash-populate-prefix/src"
  "/Users/landy/Desktop/selfProject/citybits/build/_deps/xxhash-subbuild/xxhash-populate-prefix/src/xxhash-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/landy/Desktop/selfProject/citybits/build/_deps/xxhash-subbuild/xxhash-populate-prefix/src/xxhash-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/landy/Desktop/selfProject/citybits/build/_deps/xxhash-subbuild/xxhash-populate-prefix/src/xxhash-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
