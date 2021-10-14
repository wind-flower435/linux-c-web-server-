project(sub_http_conn)

add_library(${PROJECT_NAME} http_conn.cpp)
add_library(sub_http_conn::http_conn ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/misc
        ${CMAKE_SOURCE_DIR}/html_parse
        ${CMAKE_SOURCE_DIR}/log
)

target_link_libraries( ${PROJECT_NAME}
    PRIVATE 
        sub_html_parse::html_parse
        sub_log::log
	sub_global::global
)
