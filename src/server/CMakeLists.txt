add_library(server_module server.cpp)

target_include_directories(server_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})