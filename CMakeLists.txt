project(dlt_logger)

cmake_minimum_required(VERSION 3.10)

set(DLT_LOGGER_SRC
    ./dlt_logger.cc)

include_directories(./
                    ./auto_lib/include/)

add_subdirectory(./auto_lib)

add_executable(dlt_logger ${DLT_LOGGER_SRC})
target_link_libraries(dlt_logger auto_lib pthread jsoncpp)
