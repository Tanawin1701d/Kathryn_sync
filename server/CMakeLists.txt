cmake_minimum_required(VERSION 3.24)
project(midterm)

set(CMAKE_CXX_STANDARD 17)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)
find_package(cpprestsdk REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIR})

#message("${Protobuf_LIBRARIES} dssssdddd")

FIND_PACKAGE( Boost 1.40 COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_LIBRARIES} )


LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/cppconn)

IF(WIN32)
        IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
                LINK_DIRECTORIES(${MYSQL_DIR}/lib/debug)
        ELSEIF(CMAKE_BUILD_TYPE STREQUAL "")
                LINK_DIRECTORIES(${MYSQL_DIR}/lib/opt)
        ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

        ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")

        SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn)
        IF(MYSQLCLIENT_STATIC_BINDING)
                SET(MY_TARGET_LINK_LIBRARIES ${MY_TARGET_LINK_LIBRARIES})
        ENDIF(MYSQLCLIENT_STATIC_BINDING)

        SET(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
        FIND_LIBRARY(MYSQLCPPCONN_DYNLOAD_MYSQL_LIB
                NAMES
                libmysql
                PATHS
                ${MYSQL_LIB_DIR}
                ${MYSQL_LIB_DIR}/lib/${libsuffixDist}
                ${MYSQL_LIB_DIR}/lib  #mysqlclient may be in lib for some c/c distros
                ${MYSQL_LIB_DIR}/libmysql/${libsuffixBuild}
                ${MYSQL_LIB_DIR}/client/${libsuffixBuild}
                $ENV{MYSQL_LIB_DIR}
                $ENV{MYSQL_LIB_DIR}/lib/${libsuffixDist}
                $ENV{MYSQL_LIB_DIR}/lib  #mysqlclient may be in lib for some c/c distros
                $ENV{MYSQL_LIB_DIR}/libmysql/${libsuffixBuild}
                $ENV{MYSQL_LIB_DIR}/client/${libsuffixBuild}
                $ENV{MYSQL_DIR}/lib/${libsuffixDist}
                $ENV{MYSQL_DIR}/lib  #mysqlclient may be in lib for some c/c distros
                $ENV{MYSQL_DIR}/libmysql/${libsuffixBuild}
                $ENV{MYSQL_DIR}/client/${libsuffixBuild}
                $ENV{ProgramFiles}/MySQL/*/lib/${libsuffixDist}
                $ENV{ProgramFiles}/MySQL/*/lib
                $ENV{SystemDrive}/MySQL/*/lib/${libsuffixDist}
                $ENV{SystemDrive}/MySQL/*/lib
                NO_DEFAULT_PATH
                )

ELSEIF(NOT WIN32)
        SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn-static)
        IF(MYSQLCLIENT_STATIC_BINDING)
                SET(MY_TARGET_LINK_LIBRARIES ${MY_TARGET_LINK_LIBRARIES})
        ENDIF(MYSQLCLIENT_STATIC_BINDING)

        FIND_LIBRARY(MYSQLCPPCONN_DYNLOAD_MYSQL_LIB
                NAMES
                libmysqlclient_r.so
                libmysqlclient.so
                PATHS
                ${MYSQL_LIB_DIR}
                ${MYSQL_LIB_DIR}/libmysql_r/.libs
                ${MYSQL_LIB_DIR}/lib
                ${MYSQL_LIB_DIR}/lib/mysql
                $ENV{MYSQL_LIB_DIR}
                $ENV{MYSQL_LIB_DIR}/libmysql_r/.libs
                $ENV{MYSQL_LIB_DIR}/lib
                $ENV{MYSQL_LIB_DIR}/lib/mysql
                $ENV{MYSQL_DIR}/libmysql_r/.libs
                $ENV{MYSQL_DIR}/lib
                $ENV{MYSQL_DIR}/lib/mysql
                ${MYSQL_CLIB_DIR}
                ${MYSQL_CLIB_DIR}/libmysql_r/.libs
                ${MYSQL_CLIB_DIR}/lib
                ${MYSQL_CLIB_DIR}/lib/mysql
                /usr/lib/mysql
                /usr/local/lib/mysql
                /usr/local/mysql/lib
                /usr/local/mysql/lib/mysql
                /opt/mysql/mysql/lib
                /opt/mysql/mysql/lib/mysql
                NO_DEFAULT_PATH
                )
ENDIF(WIN32)

ADD_DEFINITIONS("-DDYNLOAD_MYSQL_LIB=\"${MYSQLCPPCONN_DYNLOAD_MYSQL_LIB}\"")
MESSAGE(STATUS "MySQL dynamic load test library: ${MYSQLCPPCONN_DYNLOAD_MYSQL_LIB}")

IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
        SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)


add_executable(

        midterm

        main.cpp

        dataPool/model.h
        dataPool/model.cpp


        dataPool/dataPool.cpp
        dataPool/dataPool.h

        persistIo/ioCtrlFeed.cpp
        persistIo/ioCtrlFeed.h

        synchronizer/client.cpp
        synchronizer/client.h

        persistIo/diskCtrl.cpp
        persistIo/diskCtrl.h

        synchronizer/msg.pb.cc
        synchronizer/msg.pb.h
        controller.cpp

        controller.h

        persistIo/ioCtrlJor.cpp
        persistIo/ioCtrlJor.h

        #test/test_datapool.cpp
        #test/test_disk.cpp
        #test/test_feed.cpp
        #test/test_jor.cpp
        #test/test_client.cpp
)


TARGET_LINK_LIBRARIES( midterm PRIVATE ${Boost_LIBRARIES}
                                        Threads::Threads
                                        ${PROTOBUF_LIBRARY}
                                        cpprestsdk::cpprest
                                        mysqlcppconn

        )
