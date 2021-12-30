cmake_minimum_required(VERSION 3.10)
project(vulkanDemo)
set(CMAKE_CXX_STANDARD 17)

include_directories("libs/include")

link_directories("libs/libs")

if(WIN32)
    set(LIBS glfw3)
    set(LIBS ${LIBS} assimp)
elseif(APPLE)
    set(LIBS "libglfw.dylib" "libglfw.3.dylib" "libglfw.3.dylib")
    set(LIBS ${LIBS} "libassimpd.5.dylib" "libassimpd.dylib" "libassimpd.5.1.4.dylib")
endif(WIN32)

set(CHAPTERS
    examples
    )

set(examples
    Triangles
    )

foreach(CHAPTER ${CHAPTERS})
    foreach(DEMO ${${CHAPTER}})

        file(GLOB SOURCE "${CHAPTER}/${DEMO}/main.cpp")

        add_executable(${DEMO} ${SOURCE})
        target_link_libraries(${DEMO} ${LIBS})

    endforeach(DEMO)
endforeach(CHAPTER)