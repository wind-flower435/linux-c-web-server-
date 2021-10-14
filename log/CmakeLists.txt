project(sub_log)

add_library(${PROJECT_NAME} log.cpp)
add_library(sub_log::log ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/misc
)

target_link_libraries( ${PROJECT_NAME}
    PRIVATE 
	sub_global::global
)
