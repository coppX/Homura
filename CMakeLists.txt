cmake_minimum_required(VERSION 3.10)
project(Homura)
set(CMAKE_CXX_STANDARD 17)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

include_directories("include")
include_directories("libs")
include_directories("libs/glm")
include_directories("libs/glfw/include")
include_directories("engine/base/jobSystem/public")
include_directories("engine/platform/public")
include_directories("engine/rhi/vulkan/public")
include_directories("engine/component/public")
include_directories("engine/application")

link_directories("libs/libs")

if(WIN32)
    set(LIBS glfw3)
    set(LIBS ${LIBS} vulkan-1)
elseif(APPLE)
    set(LIBS "libglfw.3.dylib")
    set(LIBS ${LIBS} "libvulkan.1.2.154.dylib")
endif(WIN32)

file(GLOB VULKAN
    "${CMAKE_CURRENT_LIST_DIR}/engine/rhi/vulkan/private/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/engine/rhi/vulkan/private/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/engine/platform/private/*.cpp"
    )

# compile GLSL shader to SPIR-V format
file(GLOB_RECURSE SHADER ${CMAKE_CURRENT_LIST_DIR}/resources/shader/*.vert ${CMAKE_CURRENT_LIST_DIR}/resources/shader/*.frag)

foreach(shaderFile ${SHADER})
    if(WIN32)
        execute_process(COMMAND ./bin/glslangValidator.exe -V ${shaderFile} -o ${shaderFile}.spv
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    elseif(APPLE)
        execute_process(COMMAND ./bin/glslangValidator -V ${shaderFile} -o ${shaderFile}.spv
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    endif(WIN32)
endforeach(shaderFile)

set(CHAPTERS
    examples
    )

set(examples
    model
    )

foreach(CHAPTER ${CHAPTERS})
    foreach(DEMO ${${CHAPTER}})

        file(GLOB SOURCE "${CHAPTER}/${DEMO}/main.cpp")

        add_executable(${DEMO} ${SOURCE} ${VULKAN})
        target_link_libraries(${DEMO} ${LIBS})

    endforeach(DEMO)
endforeach(CHAPTER)