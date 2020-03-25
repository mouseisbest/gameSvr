add_subdirectory(protocol)
add_executable(gameSvr main.cpp) 
add_executable(testClint client.cpp) 
target_link_libraries(gameSvr protocol)
