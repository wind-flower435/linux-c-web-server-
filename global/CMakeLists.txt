project(sub_global)

add_library(${PROJECT_NAME} global.cpp)
add_library(sub_global::global ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/misc
)
