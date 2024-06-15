# set minimal version required for compilation
cmake_minimum_required(VERSION 3.25)


# set cmake standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES arm64)
#set(CMAKE_OSX_ARCHITECTURES x86_64)


# projects name and version
project(
        "LightController"
        VERSION 0.3.4
        LANGUAGES C CXX
)


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
set(VCPKG_DIR "~/Documents/.vcpkg-clion/vcpkg") # SHOULD BE CHANGED TO YOUR DIRECTORY
include(${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake)


# find vcpkg packets
find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_package(OpenGL REQUIRED)


# set file shortcuts
set(BACKEND_PATH "src/backend")
set(FRONTEND_PATH "src/frontend")


# adding all necessary file to this list for the compiler to compile it
add_executable(
        ${PROJECT_NAME}
        src/main.cpp
        ${BACKEND_PATH}/other/defines.hpp
        ${BACKEND_PATH}/PageSelect/PageSelect.cpp
        ${BACKEND_PATH}/PageSelect/PageSelect.hpp
        ${BACKEND_PATH}/ArduinoCommunication/ArduinoCommunication.cpp
        ${BACKEND_PATH}/ArduinoCommunication/ArduinoCommunication.hpp
        ${BACKEND_PATH}/BPM/BPM.cpp
        ${BACKEND_PATH}/BPM/BPM.hpp
        ${FRONTEND_PATH}/other/BaseWindow/BaseWindow.cpp
        ${FRONTEND_PATH}/other/BaseWindow/BaseWindow.hpp
        ${FRONTEND_PATH}/MasterWindow.cpp
        ${FRONTEND_PATH}/MasterWindow.hpp
        ${FRONTEND_PATH}/PageWindow/PageWindow.cpp
        ${FRONTEND_PATH}/PageWindow/PageWindow.hpp
        ${FRONTEND_PATH}/HomeWindow/HomeWindow.cpp
        ${FRONTEND_PATH}/HomeWindow/HomeWindow.hpp
        ${FRONTEND_PATH}/HomeWindow/AudioPlotWindow/AudioPlotWindow.cpp
        ${FRONTEND_PATH}/HomeWindow/AudioPlotWindow/AudioPlotWindow.hpp
        ${FRONTEND_PATH}/HomeWindow/BPMWindow/BPMWindow.cpp
        ${FRONTEND_PATH}/HomeWindow/BPMWindow/BPMWindow.hpp
        ${FRONTEND_PATH}/HomeWindow/QuickControlWindow/QuickControlWindow.cpp
        ${FRONTEND_PATH}/HomeWindow/QuickControlWindow/QuickControlWindow.hpp
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
