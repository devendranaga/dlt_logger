project(dlt_logger)

cmake_minimum_required(VERSION 3.10)

set(DLT_LOGGER_SRC
    ./src/service/dlt_service.cc)

SET(DLT_ENCDEC_SRC
    ./src/lib/dlt_enc_dec.cc)

include_directories(./
                    ./auto_lib/include/
                    ./src/lib/
                    ./src/service/
                    ./src/storage/
                    ./src/cli/)

add_subdirectory(./auto_lib)

add_executable(dlt_service ${DLT_LOGGER_SRC})
target_link_libraries(dlt_service auto_lib pthread jsoncpp dlt_enc_dec)

add_library(dlt_enc_dec ${DLT_ENCDEC_SRC})
