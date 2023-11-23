cmake_minimum_required(VERSION 3.10)

project(Sound)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
  set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
endif()

# Find SDL2 package
find_package(SDL2 REQUIRED)

# Set the source files for your project
set(SOURCE_FILES
    mednafen/dma.cpp
    mednafen/spu.cpp
    psyq_rev/psyq_spu.cpp
    psx_seq_player/lib_snd.cpp
    psx_seq_player/lib_spu.cpp
    psx_seq_player/vs_vt.c
    psx_seq_player/vs_vtc.c
    psx_seq_player/main.cpp
)

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Include SDL2 headers
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

# Link SDL2 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})

# problems with word_8001D0E8 and word_8001D710
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -fpermissive -Wno-narrowing)
endif()
