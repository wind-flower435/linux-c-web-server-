project(sub_timer)

add_library(${PROJECT_NAME} timer.cpp)
add_library(sub_timer::timer ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/misc
        ${CMAKE_SOURCE_DIR}/log
)

target_link_libraries( ${PROJECT_NAME}
    PRIVATE 
        sub_log::log
	sub_global::global
)
