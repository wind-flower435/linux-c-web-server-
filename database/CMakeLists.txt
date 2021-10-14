project(sub_database)

add_library(${PROJECT_NAME} database.cpp)
add_library(sub_database::database ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/misc
)

target_link_libraries( ${PROJECT_NAME}
    PRIVATE 
	sub_global::global
)

