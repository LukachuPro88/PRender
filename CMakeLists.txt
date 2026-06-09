cmake_minimum_required(VERSION 3.16)
project(PRender C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

find_package(Threads REQUIRED)

add_library(prender STATIC 
    src/window.c 
    src/graphics.c
    src/glad/src/glad.c
)

target_include_directories(prender PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/glad/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(prender PRIVATE ${GLFW_LIBRARIES} Threads::Threads m dl)

add_executable(engine_test test.c)

target_link_libraries(engine_test PRIVATE prender ${GLFW_LIBRARIES} Threads::Threads m dl)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

install(TARGETS prender ARCHIVE DESTINATION lib)
install(FILES include/prender.h DESTINATION include)
