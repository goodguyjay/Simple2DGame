cmake_minimum_required(VERSION 3.27)
project(Assignment2)

set(CMAKE_CXX_STANDARD 23)
set(SFML_DIR "C:/Dev/SFML-2.6.1/lib/cmake/SFML/")

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

add_executable(Assignment2 main.cpp
        Vec2.cpp
        Vec2.hpp
        Entity.cpp
        Entity.hpp
        Components.hpp
        Game.cpp
        Game.hpp
        EntityManager.cpp
        EntityManager.hpp)

target_link_libraries(Assignment2 PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

install(TARGETS Assignment2)
