# cmake v
cmake_minimum_required (VERSION 3.5)

add_subdirectory(database)
add_subdirectory(global)
add_subdirectory(html_parse)
add_subdirectory(http_conn)
add_subdirectory(log)
add_subdirectory(threadpool)
add_subdirectory(timer)

project(server)

add_executable(${PROJECT_NAME} main.cpp)


target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/misc
        ${CMAKE_SOURCE_DIR}/http_conn
        ${CMAKE_SOURCE_DIR}/log
	${CMAKE_SOURCE_DIR}/database
	${CMAKE_SOURCE_DIR}/threadpool
	${CMAKE_SOURCE_DIR}/timer
)

target_link_libraries(${PROJECT_NAME}
    sub_database::database
    sub_http_conn::http_conn
    sub_threadpool::threadpool
    sub_timer::timer
    sub_log::log
    sub_global::global
    mysqlclient
    pthread
)
