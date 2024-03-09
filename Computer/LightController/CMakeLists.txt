# set minimal version required for compilation
cmake_minimum_required(VERSION 3.25)


# projects name and version
project(
        "LightController"
        VERSION 0.3.4
        LANGUAGES C CXX
)


# set cmake standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# set general things
option(ENABLE_WARNINGS "Enable to add warnings to a target." ON)
option(ENABLE_WARNINGS_AS_ERRORS "Enable to treat warnings as errors." ON)
option(ENABLE_CLANG_TIDY "Enable to add clang tidy." ON)
option(ENABLE_CLANG_FORMAT "Enable to add clang-format." ON)


# set cmake extensions path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)


# include additional cmake files
include(Warnings)
include(Tools) # requires clang tidy and clang format
include(ConfigSafeGuards)
include(Docs)


# set vcpkg path
set(VCPKG_DIR "~/.vcpkg-clion/vcpkg") # SHOULD BE CHANGED TO YOUR DIRECTORY
include(${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake)


# find vcpkg packets
find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_package(OpenGL REQUIRED)


# adding all necessary file to this list for the compiler to compile it
add_executable(
        ${PROJECT_NAME}
        src/main.cpp
        src/backend/other/defines.hpp
        src/backend/PageSelect/PageSelect.cpp
        src/backend/PageSelect/PageSelect.hpp
        src/backend/ArduinoCommunication/ArduinoCommunication.cpp
        src/backend/ArduinoCommunication/ArduinoCommunication.hpp
        src/backend/BPM/BPM.cpp
        src/backend/BPM/BPM.hpp
        src/frontend/other/BaseWindow/BaseWindow.cpp
        src/frontend/other/BaseWindow/BaseWindow.hpp
        src/frontend/MasterWindow.cpp
        src/frontend/MasterWindow.hpp
        src/frontend/PageWindow/PageWindow.cpp
        src/frontend/PageWindow/PageWindow.hpp
        src/frontend/HomeWindow/HomeWindow.cpp
        src/frontend/HomeWindow/HomeWindow.hpp
        src/frontend/HomeWindow/AudioPlotWindow/AudioPlotWindow.cpp
        src/frontend/HomeWindow/AudioPlotWindow/AudioPlotWindow.hpp
        src/frontend/HomeWindow/BPMWindow/BPMWindow.cpp
        src/frontend/HomeWindow/BPMWindow/BPMWindow.hpp
        src/frontend/HomeWindow/QuickControlWindow/QuickControlWindow.cpp
        src/frontend/HomeWindow/QuickControlWindow/QuickControlWindow.hpp
)


# define srcs directory as part of the project
target_include_directories(${PROJECT_NAME} PUBLIC src/)


# add compile definitions
add_compile_definitions(NOMINMAX)


# add warnings to target
target_set_warnings(TARGET ${PROJECT_NAME} ENABLE ${ENABLE_WARNINGS} AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS})


# link packets to project
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE implot::implot)
