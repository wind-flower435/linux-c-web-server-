# Set the project name
project (sub_threadpool)

add_library(${PROJECT_NAME} INTERFACE)
add_library(sub_threadpool::threadpool ALIAS ${PROJECT_NAME})


