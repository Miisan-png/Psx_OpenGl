cmake_minimum_required(VERSION 3.15)
project(PSXHorrorEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(OpenGL REQUIRED)

# GLFW setup for Windows
if(WIN32)
    set(GLFW_ROOT "C:/Libraries/glfw-3.3.8")  # Adjust this path
    set(GLFW_INCLUDE_DIRS "${GLFW_ROOT}/include")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(GLFW_LIBRARIES "${GLFW_ROOT}/lib-vc2022/glfw3.lib")
    else()
        set(GLFW_LIBRARIES "${GLFW_ROOT}/lib-vc2022/glfw3.lib")
    endif()
else()
    find_package(glfw3 REQUIRED)
    set(GLFW_LIBRARIES glfw)
endif()

# Add executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    include/Shader.h
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    include/
    vendor/glad/include/
    ${GLFW_INCLUDE_DIRS}
)

# Add GLAD source
target_sources(${PROJECT_NAME} PRIVATE
    vendor/glad/src/glad.c
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    OpenGL::GL
    ${GLFW_LIBRARIES}
)

# Compiler flags for better debugging
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W3 /Od /Zi)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -g -O0 -Wall -Wextra)
    endif()
else()
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /O2 /DNDEBUG)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -DNDEBUG)
    endif()
endif()