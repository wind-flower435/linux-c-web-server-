project(sub_html_parse)

add_library(${PROJECT_NAME} html_parse.cpp)
add_library(sub_html_parse::html_parse ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/misc
        ${CMAKE_SOURCE_DIR}/database
        ${CMAKE_SOURCE_DIR}/log
)

target_link_libraries( ${PROJECT_NAME}
    PRIVATE 
        sub_database::database
        sub_log::log
	sub_global::global
)


