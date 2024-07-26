add_executable(run run.cc)
target_include_directories(run PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/games)
target_link_libraries(run PRIVATE nlohmann_json::nlohmann_json)
