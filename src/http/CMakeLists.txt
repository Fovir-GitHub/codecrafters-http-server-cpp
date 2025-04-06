add_library(http_module message.cpp)

target_link_directories(http_module PUBLIC ${CMAKE_SOURCE_DIR})