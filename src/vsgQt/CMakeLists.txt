
# create and install export header which contains export macros for libraries
#
# available arguments:
#
#    <target>        pattern for generating macro names (<target>_DECLSPEC)
#                    and install pathes (include/<target>/Export.h)
#
# In public c++ headers the generated file must be included with
#
#    #include <<target>/Export.h>
#
# and public classes be decorated with
#
#    class <target>_DECLSPEC <classname> ...
#
# @TODO: move to vsg
macro(add_library_export_header _TARGET)

    string(TOUPPER ${_TARGET} TARGET_UPPER)

    include(GenerateExportHeader)
    generate_export_header(${_TARGET}
        EXPORT_MACRO_NAME ${TARGET_UPPER}_DECLSPEC
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/${_TARGET}/Export.h
    )
    install(FILES ${CMAKE_BINARY_DIR}/include/${_TARGET}/Export.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${_TARGET})
endmacro()

set(SOURCES
    KeyboardMap.cpp
    ProxySurface.cpp
    ProxyWindow.cpp
    VulkanWindow.cpp
)

set(HEADERS
    ../../include/vsgQt/KeyboardMap.h
    ../../include/vsgQt/ProxySurface.h
    ../../include/vsgQt/ProxyWindow.h
    ../../include/vsgQt/VulkanWindow.h
)

# shared mode is automatically choosed by setting BUILD_SHARED_LIBS=ON
# POSITION_INDEPENDENT_CODE ON is set be default in shared mode
add_library(vsgQt ${SOURCES} ${HEADERS})
set_target_properties(vsgQt
    PROPERTIES
        VERSION ${vsgQt_VERSION}
        SOVERSION ${vsgQt_SOVERSION}
        CXX_STANDARD 17
)

target_include_directories(vsgQt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(vsgQt
    PUBLIC
        Qt5::Widgets
        vsg::vsg
)

add_library_export_header(vsgQt)
install(TARGETS vsgQt ${INSTALL_TARGETS_DEFAULT_FLAGS})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/vsgQt DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
