# Sample call: cmake -DCMAKE_BUILD_TYPE=Debug -DTARGET_TYPE=ARM -DPLATFORM=FW5 -DSDK_PATH=/opt/pocketbook-sdk

PROJECT(sokoban)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

SET(CMAKE_C_STANDARD 99)

IF (NOT PLATFORM)
    SET(PLATFORM FW5)
ENDIF (NOT PLATFORM)

IF (PLATFORM STREQUAL "NX")
    SET(TOOLCHAIN_PATH "${SDK_PATH}/PBSDK")
    SET(TOOLCHAIN_PREFIX "arm-linux")
    SET(TOOLCHAIN_INSTALL "")
    ADD_DEFINITIONS(-DPLATFORM_NX)
ELSEIF (PLATFORM STREQUAL "FC")
    SET(TOOLCHAIN_PATH "${SDK_PATH}/FRSCSDK")
    SET(TOOLCHAIN_PREFIX "arm-none-linux-gnueabi")
    SET(TOOLCHAIN_INSTALL "sysroot/usr")
    ADD_DEFINITIONS(-DPLATFORM_FC)
ELSE ()
    SET(TOOLCHAIN_PATH "${SDK_PATH}/SDK_481")
    SET(TOOLCHAIN_PREFIX "arm-obreey-linux-gnueabi")
    SET(TOOLCHAIN_INSTALL "sysroot/usr")
    ADD_DEFINITIONS(-DPLATFORM_FW5)
ENDIF (PLATFORM STREQUAL "NX")
#SET (CMAKE_VERBOSE_MAKEFILE ON)

IF (NOT TARGET_TYPE)
    SET(TARGET_TYPE Linux)
ENDIF (NOT TARGET_TYPE)

IF (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF (NOT CMAKE_BUILD_TYPE)

IF (NOT BUILD_FOLDER)
    SET(BUILD_FOLDER build)
ENDIF (NOT BUILD_FOLDER)

IF (TARGET_TYPE STREQUAL "ARM")
    SET(CMAKE_INSTALL_PREFIX "${TOOLCHAIN_PATH}/${TOOLCHAIN_PREFIX}/${TOOLCHAIN_INSTALL}" CACHE PATH "Install path prefix" FORCE)

    SET(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc)
    SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++)
    SET(CMAKE_LINK ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++)
    SET(CMAKE_ARR ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-arr)
    SET(CMAKE_STRIP ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-strip)

    SET(TARGET_INCLUDE "")
    SET(TARGET_LIB pthread inkview freetype z jpeg)
ELSE ()
    SET(CMAKE_INSTALL_PREFIX "${TOOLCHAIN_PATH}" CACHE PATH "Install path prefix" FORCE)

    FIND_PACKAGE(Freetype REQUIRED)
    FIND_PACKAGE(JPEG REQUIRED)
    FIND_PACKAGE(ZLIB REQUIRED)
    FIND_PACKAGE(CURL REQUIRED)
    FIND_PACKAGE(GTK2 REQUIRED)
    SET(TARGET_INCLUDE ${CMAKE_INSTALL_PREFIX}/include ${FREETYPE_INCLUDE_DIRS} ${JPEG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})
    SET(TARGET_LIB pthread inkview ${FREETYPE_LIBRARIES} ${JPEG_LIBRARIES} ${ZLIB_LIBRARIES} ${GTK2_LIBRARIES} ${CURL_LIBRARIES})

    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/${CMAKE_INSTALL_PREFIX}/lib)
ENDIF (TARGET_TYPE STREQUAL "ARM")

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")

    IF (TARGET_TYPE STREQUAL "Linux")
        MESSAGE(STATUS "Build for Linux Debug")
        SET(CMAKE_C_FLAGS_DEBUG "-DDEBUG -W -Wall -Wextra -O0 -g3 -DIVSAPP")
        SET(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -W -Wall -Wextra -O0 -g3 -DIVSAPP")
    ENDIF (TARGET_TYPE STREQUAL "Linux")

    IF (TARGET_TYPE STREQUAL "Windows")
        MESSAGE(STATUS "Build for Windows Debug")
        SET(CMAKE_C_FLAGS_DEBUG "-DDEBUG -W -Wall -Wextra -O0 -g3 -DIVSAPP")
        SET(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -W -Wall -Wextra -O0 -g3 -DIVSAPP")
    ENDIF (TARGET_TYPE STREQUAL "Windows")

    IF (TARGET_TYPE STREQUAL "ARM")
        MESSAGE(STATUS "Build for ARM Debug")
        SET(CMAKE_C_FLAGS_DEBUG "-DDEBUG -fsigned-char -W -Wall -Wextra -Wno-format-y2k -fomit-frame-pointer -O0 -g3")
        SET(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -fsigned-char -W -Wall -Wextra -Wno-format-y2k -fomit-frame-pointer -O0 -g3")
    ENDIF (TARGET_TYPE STREQUAL "ARM")

ELSE ()

    IF (TARGET_TYPE STREQUAL "Linux")
        MESSAGE(STATUS "Build for Linux Release")
        SET(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -s -Wall -O2 -DIVSAPP")
        SET(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -s -Wall -O2 -DIVSAPP")
    ENDIF (TARGET_TYPE STREQUAL "Linux")

    IF (TARGET_TYPE STREQUAL "Windows")
        MESSAGE(STATUS "Build for Windows Release")
        SET(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -s -Wall -O2 -DIVSAPP")
        SET(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -s -Wall -O2 -DIVSAPP")
    ENDIF (TARGET_TYPE STREQUAL "Windows")

    IF (TARGET_TYPE STREQUAL "ARM")
        MESSAGE(STATUS "Build for ARM Release")
        SET(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -s -fsigned-char -Wall -O2 -Wno-format-y2k -fomit-frame-pointer")
        SET(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -s -fsigned-char -Wall -O2 -Wno-format-y2k -fomit-frame-pointer")
    ENDIF (TARGET_TYPE STREQUAL "ARM")

ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_SOURCE_DIR}/${BUILD_FOLDER}/images.c
        COMMAND ${TOOLCHAIN_PATH}/bin/pbres -c ${CMAKE_SOURCE_DIR}/${BUILD_FOLDER}/images.c -4 ${CMAKE_SOURCE_DIR}/images/*.bmp)

SET(SRC_LIST
        src/sokoban.c
        src/config.c src/config.h
        src/collection.c src/collection.h
        src/tool/log.h
        src/tool/array.c src/tool/array.h
        src/tool/tablica.c src/tool/tablica.h
        src/tool/mystr.c src/tool/mystr.h
        src/tool/istream.c src/tool/istream.h
        src/tool/fileistream.c src/tool/fileistream.h
        src/tool/ostream.c src/tool/ostream.h
        src/tool/fileostream.c src/tool/fileostream.h
        src/tool/debug.c src/tool/debug.h
        src/tool/iterator.c src/tool/iterator.h
        src/tool/tableiterator.c src/tool/tableiterator.h
        src/tool/properties.c src/tool/properties.h
        )

ADD_EXECUTABLE(sokoban.app
        ${SRC_LIST}
        ${CMAKE_SOURCE_DIR}/${BUILD_FOLDER}/images.c)

INCLUDE_DIRECTORIES(${TARGET_INCLUDE})
TARGET_LINK_LIBRARIES(sokoban.app ${TARGET_LIB})

INSTALL(TARGETS sokoban.app DESTINATION bin)

