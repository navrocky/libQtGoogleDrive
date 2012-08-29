set(TARGET google_drive)

find_package(Qt4 REQUIRED QtCore QtNetwork )
include(${QT_USE_FILE})

set(HEADERS
    drive_manager.h
    session.h
    command.h
    command_oauth2.h
)

set(MOC_HEADERS
    drive_manager.h
    session.h
    command.h
    command_oauth2.h
)

set(SOURCES
    drive_manager.cpp
    session.cpp
    command.cpp
    command_oauth2.cpp
)

set(DEPS
    qjson
)

qt4_wrap_cpp(SOURCES ${MOC_HEADERS})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} ${DEPS})



