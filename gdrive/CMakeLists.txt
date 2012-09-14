set(TARGET gdrive)
project(${TARGET})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED QtCore QtNetwork QtGui)
include(${QT_USE_FILE})

set(HEADERS
    gdrive.h
)

set(MOC
    gdrive.h
)

set(SOURCES
    main.cpp
    gdrive.cpp
)

set(IFACES
)

set(RESOURCES
)

set(DEPS
    google_drive
)

qt4_wrap_cpp(SOURCES ${MOC})
qt4_wrap_ui(SOURCES ${IFACES})
qt4_add_resources(SOURCES ${RESOURCES})

add_executable(${TARGET} WIN32 ${HEADERS} ${SOURCES})
target_link_libraries(${TARGET} ${QT_LIBRARIES} ${DEPS})


