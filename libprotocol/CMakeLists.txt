file(GLOB SRC_LIST "*.cpp" "*.cc")
file(GLOB HEADERS "*.h")

aux_source_directory(./generated SRC_LIST)
include_directories(./generated)

add_library(${PROTO_TARGET} ${SRC_LIST} ${HEADERS})
target_compile_options(${PROTO_TARGET} PRIVATE -Wno-error -Wno-unused-variable)

find_package(Protobuf CONFIG REQUIRED)
target_link_libraries(${PROTO_TARGET} PUBLIC protobuf::libprotobuf)