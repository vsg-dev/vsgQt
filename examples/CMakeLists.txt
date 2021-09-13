set(SOURCES
    main.cpp
)

set(HEADERS
)

add_executable(vsgQtViewer ${MODE} ${SOURCES} ${HEADERS} ${FORMS})
target_link_libraries(vsgQtViewer
    PRIVATE
        vsgQt
)

install(TARGETS vsgQtViewer DESTINATION bin)

