cmake_minimum_required(VERSION 3.2)

project(hl_server C CXX)

SET (USER_SERVICE "user_service")
SET (PACKAGE_SERVICE "package_service")
SET (DELIVERY_SERVICE "delivery_service")
SET (EVENT_LISTENER "event_listener")

SET (STD_CXX "c++17")
SET (REDISCPP_FLAGS "-DREDISCPP_HEADER_ONLY=ON")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -std=${STD_CXX} ${REDISCPP_FLAGS}")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -std=${STD_CXX} -Wall -DNDEBUG ${REDISCPP_FLAGS}")

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)
find_package(CppKafka REQUIRED)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${USER_SERVICE} main_user.cpp 
                                 config/config.cpp 
                                 database/database.cpp
                                 database/user.cpp
                                 database/cache.cpp)

                                 target_include_directories(${USER_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${USER_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${USER_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${USER_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${USER_SERVICE} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             "rdkafka++"
                             ZLIB::ZLIB
                             CppKafka::cppkafka)



add_executable(${EVENT_LISTENER} main_event.cpp 
                                 config/config.cpp 
                                 database/database.cpp 
                                 database/user.cpp
                                 database/cache.cpp)

target_include_directories(${EVENT_LISTENER} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${EVENT_LISTENER} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${EVENT_LISTENER} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${EVENT_LISTENER} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${EVENT_LISTENER} PRIVATE 
                        ${CMAKE_THREAD_LIBS_INIT} 
                        ${Poco_LIBRARIES}
                        "PocoData"
                        "PocoDataMySQL"
                        "mysqlclient"
                        "rdkafka++"
                        ZLIB::ZLIB
                        CppKafka::cppkafka)



add_executable(${PACKAGE_SERVICE} main_package.cpp 
                                 config/config.cpp 
                                 database/database.cpp
                                 database/package.cpp)

target_include_directories(${PACKAGE_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${PACKAGE_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${PACKAGE_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${PACKAGE_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PACKAGE_SERVICE} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)


                             
add_executable(${DELIVERY_SERVICE} main_delivery.cpp 
                                 config/config.cpp 
                                 database/database.cpp
                                 database/delivery.cpp)

target_include_directories(${DELIVERY_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${DELIVERY_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${DELIVERY_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${DELIVERY_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${DELIVERY_SERVICE} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)
