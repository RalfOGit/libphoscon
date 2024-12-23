# CMakeList.txt: CMake-Projekt für "libphoscon". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)

project ("phoscon")
message("PROJECT_NAME ${PROJECT_NAME}")

#
# Target:  ${PROJECT_NAME}  =>  create phoscon.lib or libphoscon.a
#
set(COMMON_SOURCES
    src/PhosconAPI.cpp
    src/Json.cpp
    src/Logger.cpp
    src/HttpClient.cpp
    src/Url.cpp
)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

add_library(${PROJECT_NAME} STATIC ${COMMON_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
target_link_libraries(phoscon)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    LIB_NAMESPACE=libphoscon
)

#
# Target:  ${PROJECT_NAME}_test  =>  create phoscon_test.exe
#
set(TEST_SOURCES
    src/Test.cpp
)
set(TEST_INCLUDE_DIR ${INCLUDE_DIR})

add_executable(${PROJECT_NAME}_test ${TEST_SOURCES})
add_dependencies(${PROJECT_NAME}_test ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}_test PUBLIC ${TEST_INCLUDE_DIR})

target_compile_definitions(${PROJECT_NAME}_test PRIVATE
    LIB_NAMESPACE=libphoscon
)

if (MSVC)
target_link_libraries(${PROJECT_NAME}_test phoscon.lib ws2_32.lib Iphlpapi.lib)
else()
target_link_libraries(${PROJECT_NAME}_test ${CMAKE_SOURCE_DIR}/build/libphoscon.a)
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES 
    OUTPUT_NAME ${PROJECT_NAME}_test
    ARCHIVE_OUTPUT_NAME ${PROJECT_NAME}
)
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_test RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
