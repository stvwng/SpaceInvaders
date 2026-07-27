cmake_minimum_required(VERSION 3.20)
project(SpaceInvaders LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Emit compile_commands.json so clangd (Cursor, VS Code) gets real diagnostics
# instead of guessing at include paths.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# --- Dependencies ---------------------------------------------------------
# There is also a hand-installed SFML 2.6.1 at /Library/Frameworks on this
# machine (left over from an earlier attempt). Prefer the Homebrew keg, which
# is versioned and upgradeable, over an unmanaged framework that will rot.
set(CMAKE_FIND_FRAMEWORK LAST)

# sfml@2 is keg-only in Homebrew, so it is not on the default search path.
# Ask brew where it lives rather than making every developer set
# CMAKE_PREFIX_PATH by hand.
if(APPLE)
    find_program(BREW_EXECUTABLE brew)
    if(BREW_EXECUTABLE)
        execute_process(
            COMMAND ${BREW_EXECUTABLE} --prefix sfml@2
            OUTPUT_VARIABLE SFML2_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(SFML2_PREFIX)
            list(APPEND CMAKE_PREFIX_PATH "${SFML2_PREFIX}")
        endif()
    endif()
endif()

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

# --- Options --------------------------------------------------------------
option(SPACEINVADERS_SANITIZE "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(SPACEINVADERS_FULLSCREEN "Launch the game fullscreen instead of windowed" OFF)
option(SPACEINVADERS_DEBUG_LOG "Compile the diagnostic logging blocks" OFF)

add_library(spaceinvaders_flags INTERFACE)
target_compile_options(spaceinvaders_flags INTERFACE
    -Wall -Wextra -Wpedantic
)

if(SPACEINVADERS_FULLSCREEN)
    target_compile_definitions(spaceinvaders_flags INTERFACE SPACEINVADERS_FULLSCREEN)
endif()

if(SPACEINVADERS_DEBUG_LOG)
    target_compile_definitions(spaceinvaders_flags INTERFACE SPACEINVADERS_DEBUG_LOG)
endif()

if(SPACEINVADERS_SANITIZE)
    target_compile_options(spaceinvaders_flags INTERFACE
        -fsanitize=address,undefined -fno-omit-frame-pointer -g
    )
    target_link_options(spaceinvaders_flags INTERFACE
        -fsanitize=address,undefined
    )
endif()

# --- Game library ---------------------------------------------------------
# Everything except main() lives in a static library. The executable and the
# test binary both link it, so tests exercise the real game code rather than a
# copy of it.
#
# GLOB is normally discouraged because CMake cannot know when a new file
# appears. CONFIGURE_DEPENDS makes the build system re-run the glob on every
# build, which trades a little configure time for not having to edit this file
# every time a class is added.
file(GLOB SPACEINVADERS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list(REMOVE_ITEM SPACEINVADERS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/SpaceInvaders.cpp")

add_library(spaceinvaders_lib STATIC ${SPACEINVADERS_SOURCES})
target_include_directories(spaceinvaders_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(spaceinvaders_lib
    PUBLIC sfml-graphics sfml-window sfml-system sfml-audio
    PRIVATE spaceinvaders_flags
)

# --- Executable -----------------------------------------------------------
add_executable(SpaceInvaders SpaceInvaders.cpp)
target_link_libraries(SpaceInvaders PRIVATE spaceinvaders_lib spaceinvaders_flags)

# --- Tests ----------------------------------------------------------------
# The tests link spaceinvaders_lib, so they exercise the real game code.
# Everything covered here is deliberately silent and windowless: no test
# constructs SoundEngine or opens a RenderWindow, so `ctest` can run anywhere
# without making noise or stealing focus.
enable_testing()

add_library(doctest INTERFACE)
target_include_directories(doctest SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

file(GLOB SPACEINVADERS_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")

add_executable(SpaceInvadersTests ${SPACEINVADERS_TEST_SOURCES})
target_link_libraries(SpaceInvadersTests PRIVATE spaceinvaders_lib doctest spaceinvaders_flags)
target_compile_definitions(SpaceInvadersTests PRIVATE
    SPACEINVADERS_TEST_FIXTURES="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
    SPACEINVADERS_REPO_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_test(NAME unit COMMAND SpaceInvadersTests)

# --- Benchmark ------------------------------------------------------------
# Headless timing harness for the simulation. Built by default so it cannot
# silently rot, but never run by ctest -- it takes seconds, not milliseconds.
add_executable(SpaceInvadersBench bench/benchmark.cpp)
target_link_libraries(SpaceInvadersBench PRIVATE spaceinvaders_lib spaceinvaders_flags)

# --- Assets ---------------------------------------------------------------
# Every asset path in the source is relative to the working directory
# ("graphics/background.png", "world/level1", ...). Copy the asset trees next
# to the binary so the game can be launched from the build directory.
set(SPACEINVADERS_ASSET_DIRS graphics sound fonts world)
foreach(asset_dir IN LISTS SPACEINVADERS_ASSET_DIRS)
    add_custom_command(TARGET SpaceInvaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/${asset_dir}"
            "$<TARGET_FILE_DIR:SpaceInvaders>/${asset_dir}"
        COMMENT "Copying ${asset_dir}/ next to the binary"
        VERBATIM
    )
endforeach()
