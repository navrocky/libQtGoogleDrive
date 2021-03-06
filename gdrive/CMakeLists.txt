set(TARGET gdrive)
project(${TARGET})

add_definitions("-std=c++0x")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Boost REQUIRED COMPONENTS
    program_options
)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Qt4 REQUIRED QtCore QtNetwork QtGui)
include(${QT_USE_FILE})

set(HEADERS
    gdrive.h
    tools.h
)

set(MOC
    gdrive.h
    tools.h
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
target_link_libraries(${TARGET} ${Boost_LIBRARIES} ${QT_LIBRARIES} ${DEPS})


