cmake_minimum_required (VERSION 3.8)

project ("JPEG_ENC")
set (CMAKE_CXX_STANDARD 11)

file( GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.*" )

file( GLOB_RECURSE THIRD_PARTY_SOURCES "${PROJECT_SOURCE_DIR}/thirdparty/*.cpp" )
file( GLOB_RECURSE THIRD_PARTY_HEADERS "${PROJECT_SOURCE_DIR}/thirdparty/*.h" )

include_directories(
	${PROJECT_SOURCE_DIR}/thirdparty/TinyTIFF
)

add_executable (JPEG_ENC 
	${THIRD_PARTY_SOURCES}
	${THIRD_PARTY_HEADERS}
	${SOURCES}
)