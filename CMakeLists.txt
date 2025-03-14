cmake_minimum_required(VERSION 3.23)

project(the-war)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -pthread -O3")
add_compile_options(-fdiagnostics-color=always)

find_package(httplib CONFIG REQUIRED)
find_package(OpenSSL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(the-war 
  src/main.cc
  src/server/server_frame.cc
  src/utils/utils.cc
  src/field/field.cc
  src/dtos/unit.cc
  src/dtos/army.cc
)

target_link_libraries(the-war PRIVATE 
  httplib::httplib
  OpenSSL::SSL
  OpenSSL::Crypto
  nlohmann_json::nlohmann_json
)

target_include_directories(the-war PUBLIC "src")
