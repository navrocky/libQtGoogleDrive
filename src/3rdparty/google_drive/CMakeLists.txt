set(TARGET google_drive)

find_package(Qt4 REQUIRED QtCore QtNetwork )
include(${QT_USE_FILE})

#add_definitions(-DQJSON_MAKEDLL)

set(HEADERS
    drive_manager.h
)


set(MOC_HEADERS
    drive_manager.h
)

set(SOURCES
    drive_manager.cpp
)

qt4_wrap_cpp(SOURCES ${MOC_HEADERS})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} )



