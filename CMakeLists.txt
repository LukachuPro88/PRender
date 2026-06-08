cmake_minimum_required(VERSION 3.16)
project(PRender C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find GLFW dependency natively
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# Define your source file configurations
add_library(prender STATIC 
    src/window.c 
    src/graphics.c
    src/glad/src/glad.c
)

# Expose include targets
target_include_directories(prender PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/glad/include>
    $<INSTALL_INTERFACE:include>
)

# Link GLFW to PRender internally
target_link_libraries(prender PRIVATE ${GLFW_LIBRARIES} m dl)

# SYSTEM INSTALLATION RULES
install(TARGETS prender ARCHIVE DESTINATION lib)
install(FILES include/prender.h DESTINATION include)
