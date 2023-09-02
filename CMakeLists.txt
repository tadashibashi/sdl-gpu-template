cmake_minimum_required(VERSION 3.26)
project(sdlgpu-template)

set(CMAKE_CXX_STANDARD 20)

if (EMSCRIPTEN)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s FULL_ES3=1")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

endif()

find_package(EnTT CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(spdlog REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(lib)

target_link_libraries(${PROJECT_NAME} PRIVATE
    EnTT::EnTT $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> SDL2::SDL2-static glm::glm  $<TARGET_NAME_IF_EXISTS:OpenGL::GL> spdlog::spdlog SDL_gpu)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/sdl-gpu/include)