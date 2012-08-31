set(TARGET google_drive)

project(${TARGET})

find_package(Qt4 REQUIRED QtCore QtNetwork )
include(${QT_USE_FILE})

FIND_PACKAGE (QJSON REQUIRED)

INCLUDE_DIRECTORIES (
    ${QJSON_INCLUDE_DIR}
)

set(HEADERS
    drive_manager.h
    session.h
    file_info.h
    command.h
    command_oauth2.h
    command_file_list.h
)

set(MOC_HEADERS
    drive_manager.h
    session.h
    command.h
    command_oauth2.h
    command_file_list.h
)

set(SOURCES
    drive_manager.cpp
    session.cpp
    file_info.cpp
    command.cpp
    command_oauth2.cpp
    command_file_list.cpp
)

set(DEPS
    qjson
)

qt4_wrap_cpp(SOURCES ${MOC_HEADERS})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} ${DEPS} ${QJSON_LIBRARIES})



