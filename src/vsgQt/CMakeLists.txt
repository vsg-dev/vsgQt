
# create and install export header which contains export macros for libraries
#
# available arguments:
#
#    <target>        pattern for generating macro names (<target>_DECLSPEC)
#                    and install paths (include/<target>/Export.h)
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
        EXPORT_FILE_NAME ${VSGQT_BINARY_DIR}/include/${_TARGET}/Export.h
    )
    install(FILES ${VSGQT_BINARY_DIR}/include/${_TARGET}/Export.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${_TARGET})

    # pass the creation mode to the corresponding target in the cmake
    if(NOT BUILD_SHARED_LIBS)
        target_compile_definitions(${_TARGET} INTERFACE ${TARGET_UPPER}_STATIC_DEFINE)
    endif()
endmacro()

set(SOURCES
    KeyboardMap.cpp
    Window.cpp
    Viewer.cpp
)

set(HEADERS
    ../../include/vsgQt/KeyboardMap.h
    ../../include/vsgQt/Window.h
    ../../include/vsgQt/Viewer.h
)

# shared mode is automatically chosen by setting BUILD_SHARED_LIBS=ON
# POSITION_INDEPENDENT_CODE ON is set by default in shared mode
add_library(vsgQt ${SOURCES} ${HEADERS})

# add definitions to enable building vsgQt as part of submodule
add_library(vsgQt::vsgQt ALIAS vsgQt)
set(vsgQt_FOUND TRUE CACHE INTERNAL "vsgQt found.")
set(CMAKE_DISABLE_FIND_PACKAGE_vsgQt TRUE CACHE INTERNAL "Disable find_package(vsgQt) as it's not necessary.")

set_target_properties(vsgQt
    PROPERTIES
        VERSION ${VSGQT_VERSION}
        SOVERSION ${VSGQT_SOVERSION}
        CXX_STANDARD 17
)

target_include_directories(vsgQt
    PUBLIC
        $<BUILD_INTERFACE:${VSGQT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${VSGQT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(vsgQt
    PUBLIC
        ${QT_PACKAGE_NAME}::Widgets
        vsg::vsg
)

add_library_export_header(vsgQt)

install(TARGETS vsgQt ${INSTALL_TARGETS_DEFAULT_FLAGS})
install(DIRECTORY ${VSGQT_SOURCE_DIR}/include/vsgQt DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

vsg_add_cmake_support_files(
    CONFIG_TEMPLATE vsgQtConfig.cmake.in
)
