# - Functions to help assemble a standalone Qt4 application.
# A collection of CMake utility functions useful for deploying
# Qt4 applications.
#
# The following functions are provided by this module:
#	write_qt4_conf
#	fixup_qt4_bundle
#	install_qt4_app
# Requires CMake 2.6 or greater and depends on BundleUtilities.cmake.
#
#  WRITE_QT4_CONF(<qt_conf_dir> <qt_conf_contents>)
# Writes a qt.conf file with the given contents into the given directory.
#
# TODO: document variables
#
#  FIXUP_QT4_BUNDLE(<app> [<qtplugins> <libs> <dirs> <plugins_dir> <request_qt_conf>])
# Copies Qt plugins, writes a Qt configuration file (if needed) and fixes up a
# Qt4 executable using BundleUtilities.
#
# TODO: document variables
#
#  INSTALL_QT4_APP(<executable> <qtplugins> [<libs> <dirs> <plugins_dir> <request_qt_conf>])
# Installs plugins, writes a Qt configuration file (if needed) and fixes up a
# Qt4 executable using BundleUtilties.
#
# TODO: document variables

#=============================================================================
# Copyright 2011 Mike McQuaid <mike@mikemcquaid.com>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# The functions defined in this file depend on the fixup_bundle function
# (and possibly others) found in BundleUtilities.cmake
#
include(BundleUtilities)

macro(write_qt4_conf qt_conf_dir qt_conf_contents)
        set(qt_conf_path "${qt_conf_dir}/qt.conf")
        message(STATUS "Writing ${qt_conf_path}")
        file(WRITE "${qt_conf_path}" "${qt_conf_contents}")
endmacro()

macro(resolve_qt4_paths paths_var)
        set(app_path ${ARGV1})

        set(paths_resolved)
        foreach(path ${${paths_var}})
                if(EXISTS "${path}")
                        list(APPEND paths_resolved "${path}")
                else()
                        if(${app_path})
                                list(APPEND paths_resolved "${app_path}/${path}")
                        else()
                                list(APPEND paths_resolved "\${CMAKE_INSTALL_PREFIX}/${path}")
                        endif()
                endif()
        endforeach()
        set(${paths_var} ${paths_resolved})
endmacro()

function(fixup_qt4_bundle app qtplugins)
        set(libs ${ARGV2})
        set(dirs ${ARGV3})
        set(plugins_dir ${ARGV4})
        set(request_qt_conf ${ARGV5})

        message(STATUS "fixup_qt4_bundle")
        message(STATUS "  app='${app}'")
        message(STATUS "  qtplugins='${qtplugins}'")
        message(STATUS "  libs='${libs}'")
        message(STATUS "  dirs='${dirs}'")
        message(STATUS "  plugins_dir='${plugins_dir}'")
        message(STATUS "  request_qt_conf='${request_qt_conf}'")

        if(QT_LIBRARY_DIR)
                list(APPEND dirs "${QT_LIBRARY_DIR}")
        endif()

        if(APPLE)
                set(qt_conf_dir "${app}/Contents/Resources")
                set(app_path "${app}")
                set(write_qt_conf TRUE)
        else()
                get_filename_component(app_path "${app}" PATH)
                set(qt_conf_dir "${app_path}")
                set(write_qt_conf ${request_qt_conf})
        endif()

        foreach(plugin ${qtplugins})
                set(installed_plugin_path "")
                install_qt4_plugin("${plugin}" "${plugins_dir}" "${app}" 1 installed_plugin_path)
                list(APPEND libs ${installed_plugin_path})
        endforeach()

        foreach(lib ${libs})
                if(NOT EXISTS "${lib}")
                        message(FATAL_ERROR "Library does not exist: ${lib}")
                endif()
        endforeach()

        resolve_qt4_paths(libs "${app_path}")
        resolve_qt4_paths(dirs "${app_path}")

        if(qtplugins AND write_qt_conf)
                set(qt_conf_contents "[Paths]\nPlugins = ${plugins_dir}")
                write_qt4_conf("${qt_conf_dir}" "${qt_conf_contents}")
        endif()

        fixup_bundle("${app}" "${libs}" "${dirs}")
endfunction()

macro(install_qt4_plugin_path plugin plugins_dir app copy installed_path_var configurations)
        if(EXISTS "${plugin}")
                if(plugins_dir)
                        set(plugins_subdir "${plugins_dir}")
                else()
                        if (APPLE)
                                set(plugins_subdir "PlugIns")
                        else()
                                set(plugins_subdir "plugins")
                        endif()
                endif()
                if(APPLE)
                        set(plugins_path "${app}/Contents/${plugins_subdir}")
                else()
                        get_filename_component(app_path "${app}" PATH)
                        set(plugins_path "${app_path}/${plugins_subdir}")
                endif()

                set(plugin_group "")

                get_filename_component(plugin_path "${plugin}" PATH)
                get_filename_component(plugin_parent_path "${plugin_path}" PATH)
                get_filename_component(plugin_parent_dir_name "${plugin_parent_path}" NAME)
                get_filename_component(plugin_name "${plugin}" NAME)
                string(TOLOWER "${plugin_parent_dir_name}" plugin_parent_dir_name)

                if("${plugin_parent_dir_name}" STREQUAL "plugins")
                        get_filename_component(plugin_group "${plugin_path}" NAME)
                        set(${plugin_group_var} "${plugin_group}")
                endif()
                set(plugins_path "${plugins_path}/${plugin_group}")

                if(${copy})
                        file(MAKE_DIRECTORY "${plugins_path}")
                        file(COPY "${plugin}" DESTINATION "${plugins_path}")
                else()
                        if(configurations AND (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE))
                                install(FILES "${plugin}" DESTINATION "${plugins_path}" CONFIGURATIONS ${configurations})
                        else()
                                install(FILES "${plugin}" DESTINATION "${plugins_path}")
                        endif()
                endif()
                set(${installed_path_var} ${${installed_path_var}} "${plugins_path}/${plugin_name}")
        endif()
endmacro()

macro(install_qt4_plugin plugin plugins_dir app copy installed_path_var)
        if(EXISTS "${plugin}")
                install_qt4_plugin_path("${plugin}" "${plugins_dir}" "${app}" "${copy}" "${installed_path_var}" "")
        else()
                string(TOUPPER "QT_${plugin}_PLUGIN" plugin_var)
                set(plugin_release "${${plugin_var}_RELEASE}")
                set(plugin_debug "${${plugin_var}_DEBUG}")
                if(NOT EXISTS "${plugin_release}" AND NOT EXISTS "${plugin_debug}")
                        message(WARNING "Qt plugin \"${plugin}\" not recognized or found.")
                endif()
                install_qt4_plugin_path("${plugin_release}" "${plugins_dir}" "${app}" "${copy}" "${installed_path_var}" "Release|RelWithDebInfo|MinSizeRel")
                install_qt4_plugin_path("${plugin_debug}" "${plugins_dir}" "${app}" "${copy}" "${installed_path_var}" "Debug")
        endif()
endmacro()

function(install_qt4_app executable)
        set(qtplugins ${ARGV1})
        set(libs ${ARGV2})
        set(dirs ${ARGV3})
        set(plugins_dir ${ARGV4})
        set(request_qt_conf ${ARGV5})
        if(QT_LIBRARY_DIR)
                list(APPEND dirs "${QT_LIBRARY_DIR}")
        endif()

        get_filename_component(executable_absolute "${executable}" ABSOLUTE)
        gp_file_type("${executable_absolute}" "${QT_QTCORE_LIBRARY}" qtcore_type)
        if(qtcore_type STREQUAL "system")
                set(qt_plugins_dir "")
        endif()

        if(NOT qtplugins AND QT_LIBRARIES_PLUGINS)
                set(qtplugins_default TRUE)
                set(qtplugins "${QT_LIBRARIES_PLUGINS}")
        endif()

        foreach(plugin ${qtplugins})
                set(installed_plugin_paths "")
                install_qt4_plugin("${plugin}" "${plugins_dir}" "${executable}" 0 installed_plugin_paths)
                list(APPEND libs ${installed_plugin_paths})
        endforeach()

        resolve_qt4_paths(libs "")
        resolve_qt4_paths(dirs "")

        install(CODE
                " INCLUDE( \"${CMAKE_ROOT}/Modules/DeployQt4.cmake\" )
                SET( BU_CHMOD_BUNDLE_ITEMS TRUE )
                FIXUP_QT4_BUNDLE( \"\${CMAKE_INSTALL_PREFIX}/${executable}\" \"\" \"${libs}\" \"${dirs}\" \"${plugins_dir}\" \"${request_qt_conf}\" ) "
        )
endfunction()
