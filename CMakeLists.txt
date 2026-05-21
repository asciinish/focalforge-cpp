cmake_minimum_required(VERSION 3.15)
project(image_editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs highgui)

add_executable(image_editor
    src/main.cpp
    src/ops.cpp
    src/ui.cpp
    src/io.cpp
)

target_include_directories(image_editor PRIVATE src ${OpenCV_INCLUDE_DIRS})
target_link_libraries(image_editor PRIVATE ${OpenCV_LIBS})

if(MSVC)
    target_compile_options(image_editor PRIVATE /W4)
else()
    target_compile_options(image_editor PRIVATE -Wall -Wextra -Wpedantic)
endif()