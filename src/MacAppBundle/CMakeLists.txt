set(PYTHON_DIR_BASENAME python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR})

if(PYTHON_LIBRARY MATCHES "(.*Python\\.framework).*")
    #framework
    set(PYTHON_DIR ${CMAKE_MATCH_1}/Versions/Current/lib/${PYTHON_DIR_BASENAME})
else()
    #unix
    get_filename_component(PYTHON_DIR ${PYTHON_LIBRARY} PATH)
    set(PYTHON_DIR ${PYTHON_DIR}/${PYTHON_DIR_BASENAME})
endif()

set(WEBKIT_FRAMEWORK_DIR "")
if (Qt5WebKitWidgets_FOUND)
    #qtwebkitwidgets references qtwebkit with a relative path,
    #so we need to add its location to the search paths
    get_filename_component(WEBKIT_LIB_DIR ${Qt5WebKitWidgets_DIR} PATH)
    get_filename_component(WEBKIT_LIB_DIR ${WEBKIT_LIB_DIR} PATH)
    set(WEBKIT_FRAMEWORK_DIR "${WEBKIT_LIB_DIR}/QtWebKit.framework/Versions/5")
endif()

install(CODE "execute_process(COMMAND
    ${CMAKE_COMMAND} -E copy_directory ${PYTHON_DIR} ${CMAKE_INSTALL_LIBDIR}/${PYTHON_DIR_BASENAME}
)")

if(HOMEBREW_PREFIX)
    #Homebrew installs python dependencies to a site dir in prefix/libexec
    #and installs a .pth file containing its path to the HOMEBREW_PREFIX site dir.

    file(GLOB HOMEBREW_PTH_FILES "${PYTHON_DIR}/site-packages/homebrew*.pth")

    foreach(PTH_FILE ${HOMEBREW_PTH_FILES})
        file(READ ${PTH_FILE} ADDITIONAL_DIR)

	string(STRIP "${ADDITIONAL_DIR}" ADDITIONAL_DIR)
        string(REGEX REPLACE "^${HOMEBREW_PREFIX}/Cellar/([A-Za-z0-9_]+).*$" "\\1" LIB_NAME ${ADDITIONAL_DIR})
        string(REGEX REPLACE ".*libexec(.*)/site-packages" "libexec/${LIB_NAME}\\1" NEW_SITE_DIR ${ADDITIONAL_DIR})

        install(DIRECTORY ${ADDITIONAL_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/${NEW_SITE_DIR})

        #update the paths of the .pth files copied into the bundle
        get_filename_component(PTH_FILENAME ${PTH_FILE} NAME)
        install(CODE
            "file(WRITE
                 ${CMAKE_INSTALL_LIBDIR}/${PYTHON_DIR_BASENAME}/site-packages/${PTH_FILENAME}
		 \"../../../${NEW_SITE_DIR}/site-packages\"
            )"
        )
    endforeach(PTH_FILE)
endif()

if(BUILD_QT5)
  set(QT_PLUGINS_DIR "${Qt5Core_DIR}/../../../plugins")
  execute_process(COMMAND "xcode-select" "--print-path"
                  OUTPUT_VARIABLE XCODE_PATH
                  ERROR_QUIET
		 )
  string(STRIP ${XCODE_PATH} XCODE_PATH)

  set(XCTEST_PATH "${XCODE_PATH}/Platforms/MacOSX.platform/Developer/Library/Frameworks/XCTest.framework/Versions/Current")

  # add qt assistant to bundle
  install(PROGRAMS "${Qt5Core_DIR}/../../../libexec/Assistant.app/Contents/MacOS/Assistant" DESTINATION ${CMAKE_INSTALL_PREFIX}/MacOS)
endif(BUILD_QT5)

  # add QtWebEngineProcess to bundle
  if(BUILD_WEB)
  install(PROGRAMS "${Qt5Core_DIR}/../../../lib/QtWebEngineCore.framework/Versions/5/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess" DESTINATION ${CMAKE_INSTALL_PREFIX}/MacOS)

  # add locales to bundle
  	file(GLOB _locales_files RELATIVE "${Qt5Core_DIR}/../../../lib/QtWebEngineCore.framework/Versions/5/Resources/qtwebengine_locales" "${Qt5Core_DIR}/../../../lib/QtWebEngineCore.framework/Versions/5/Resources/qtwebengine_locales/*")
  	foreach(_locales_file ${_locales_files})
        get_filename_component(_resolved_file "${Qt5Core_DIR}/../../../lib/QtWebEngineCore.framework/Versions/5/Resources/qtwebengine_locales/${_locales_file}" REALPATH)
        list(APPEND _locales_resolved_files ${_resolved_file})
  	endforeach()
  	install(FILES ${_locales_resolved_files} DESTINATION "${CMAKE_INSTALL_PREFIX}/MacOS/qtwebengine_locales")
  
  # add pak
  file(GLOB _pak_files RELATIVE "${Qt5Core_DIR}/../../../lib/QtWebEngineCore.framework/Versions/5/Resources/" "${Qt5Core_DIR}/../../../lib/QtWebEngineCore.framework/Versions/5/Resources/*.pak")
  foreach(_pak_file ${_pak_files})
        get_filename_component(_pak_resolved_file "${Qt5Core_DIR}/../../../lib/QtWebEngineCore.framework/Versions/5/Resources/${_pak_file}" REALPATH)
        list(APPEND _pak_resolved_files ${_pak_resolved_file})
  endforeach()
  install(FILES ${_pak_resolved_files} DESTINATION "${CMAKE_INSTALL_PREFIX}/")

  # add icudtl.dat
 
  install(PROGRAMS "${Qt5Core_DIR}/../../../lib/QtWebEngineCore.framework/Versions/5/Resources/icudtl.dat" DESTINATION ${CMAKE_INSTALL_PREFIX}/)
  	install(PROGRAMS "${Qt5Core_DIR}/../../../lib/QtWebEngineCore.framework/Versions/5/Resources/QtWebEngineCore.prl" DESTINATION ${CMAKE_INSTALL_PREFIX}/)
  endif(BUILD_WEB)


# Ensure the actual plugin files are installed instead of symlinks.
file(GLOB _subdirs RELATIVE "${QT_PLUGINS_DIR}" "${QT_PLUGINS_DIR}/*")

foreach(_subdir ${_subdirs})
    if(IS_DIRECTORY "${QT_PLUGINS_DIR}/${_subdir}")
        set(_resolved_files "")
        file(GLOB _plugin_files RELATIVE "${QT_PLUGINS_DIR}/${_subdir}" "${QT_PLUGINS_DIR}/${_subdir}/*")

        foreach(_plugin_file ${_plugin_files})
            get_filename_component(_resolved_file "${QT_PLUGINS_DIR}/${_subdir}/${_plugin_file}" REALPATH)
            list(APPEND _resolved_files ${_resolved_file})
        endforeach()

        install(FILES ${_resolved_files} DESTINATION "${CMAKE_INSTALL_LIBDIR}/qtplugins/${_subdir}")
    endif()
endforeach()

#files installed by homebrew do not have write permission for regular user
install(CODE "execute_process(COMMAND chmod -R a+w ${CMAKE_INSTALL_LIBDIR})")

get_filename_component(APP_PATH ${CMAKE_INSTALL_PREFIX} PATH)

file(GLOB CONFIG_ICU "/usr/local/Cellar/icu4c*/*/lib")

execute_process(
       COMMAND find /usr/local/Cellar/nglib -name MacOS
       OUTPUT_VARIABLE CONFIG_NGLIB)

install(CODE 
    "message(STATUS \"Making bundle relocatable...\")
    # The top-level CMakeLists.txt should prevent multiple package manager
    # prefixes from being set, so the lib path will resolve correctly...
    execute_process(
        COMMAND python2.7
        ${CMAKE_SOURCE_DIR}/src/Tools/MakeMacBundleRelocatable.py
        ${APP_PATH} ${HOMEBREW_PREFIX}${MACPORTS_PREFIX}/lib ${CONFIG_ICU} /usr/local/opt ${CONFIG_NGLIB} ${Qt5Core_DIR}/../../.. ${XCTEST_PATH} ${HOMEBREW_PREFIX}/opt/llvm/lib ${WEBKIT_FRAMEWORK_DIR}
    )"
)
