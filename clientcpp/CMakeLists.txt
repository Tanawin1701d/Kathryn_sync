cmake_minimum_required(VERSION 3.24)
project(clientcpp)


find_package(CURL REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(clientcpp main.cpp)


TARGET_LINK_LIBRARIES( clientcpp PRIVATE  ${CURL_LIBRARIES} )

