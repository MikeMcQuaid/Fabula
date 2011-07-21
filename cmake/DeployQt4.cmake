# - Functions to help assemble a standalone Qt4 application.
# A collection of CMake utility functions useful for deploying
# Qt4 applications.
#
# The following functions are provided by this module:
#   write_qt4_conf
#   fixup_qt4_bundle
#   install_qt4_app
# Requires CMake 2.6 or greater and depends on BundleUtilities.cmake.
#
#  WRITE_QT4_CONF(<qt_conf_dir> <qt_conf_contents>)
# Writes a qt.conf file with the given contents into the given directory.
#
#  FIXUP_QT4_BUNDLE(<app> [<qtplugins> <libs> <dirs> <plugins_dir>])
# Copies Qt plugins, writes a Qt configuration file (if needed) and fixes up a
# Qt4 executable using BundleUtilities.
#
#  INSTALL_QT4_APP(<executable> <qtplugins> [<libs> <dirs> <plugins_dir> <request_qt_conf])
# Installs plugins, writes a Qt configuration file (if needed) and fixes up a
# Qt4 executable using BundleUtilties.

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
#get_filename_component(DeployQt4_cmake_dir "${CMAKE_CURRENT_LIST_FILE}" PATH)
#include("${DeployQt4_cmake_dir}/BundleUtilities.cmake")
include(BundleUtilities)

function(write_qt4_conf qt_conf_dir qt_conf_contents)
	set(qt_conf_path "${qt_conf_dir}/qt.conf")
	message(STATUS "Writing ${qt_conf_path}")
	file(WRITE "${qt_conf_path}" "${qt_conf_contents}")
endfunction(write_qt4_conf)


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
		if(NOT plugins_dir)
			set(plugins_dir "PlugIns")
		endif()
		set(plugins_path "${app}/Contents/${plugins_dir}")
		set(qt_conf_dir "${app}/Contents/Resources")
		set(write_qt_conf TRUE)
	else()
		if(NOT plugins_dir)
			set(plugins_dir "plugins")
		endif()
		set(plugins_dir ${plugin_inst_dir})
		get_filename_component(app_path "${app}" PATH)
		set(plugins_path "${app_path}/${plugins_dir}")
		set(qt_conf_dir "")
		set(write_qt_conf ${request_qt_conf})
	endif()

	foreach(plugin ${qtplugins})
		if(EXISTS "${plugin}")
			set(plugin_group "")

			get_filename_component(plugin_path "${plugin}" PATH)
			get_filename_component(plugin_parent_path "${plugin_path}" PATH)
			get_filename_component(plugin_parent_dir_name "${plugin_parent_path}" NAME)
			get_filename_component(plugin_name "${plugin}" NAME)
			string(TOLOWER "${plugin_parent_dir_name}" plugin_parent_dir_name)

			if("${plugin_parent_dir_name}" STREQUAL "plugins")
				get_filename_component(plugin_group "${plugin_path}" NAME)
				message(STATUS "Matched ${plugin_name} to plugin group ${plugin_group}")
			endif()

			set(plugin_path "${plugins_path}/${plugin_group}")
			message(STATUS "Copying ${plugin_name} to ${plugin_path}")
			file(MAKE_DIRECTORY "${plugin_path}")
			file(COPY "${plugin}" DESTINATION "${plugin_path}")

			list(APPEND libs "${plugin_path}/${plugin_name}")
		else()
			message(FATAL_ERROR "Plugin ${plugin} not found")
		endif()
	endforeach()

	if(qtplugins AND write_qt_conf)
		set(qt_conf_contents "[Paths]\nPlugins = ${plugins_dir}")
		write_qt4_conf("${qt_conf_dir}" "${qt_conf_contents}")
	endif()

	fixup_bundle("${app}" "${libs}" "${dirs}")
endfunction(fixup_qt4_bundle)


function(install_qt4_app executable)
	set(qtplugins ${ARGV1})
	set(libs ${ARGV2})
	set(dirs ${ARGV3})
	set(plugins_dir ${ARGV4})
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
		if(NOT EXISTS "${plugin}")
			string(TOUPPER "QT_${plugin}_PLUGIN" plugin_var)
			set(release_regex "Release|RelWithDebInfo|MinSizeRel")
			if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
				if(CMAKE_CONFIGURATION_TYPES MATCHES "${release_regex}" OR CMAKE_BUILD_TYPE MATCHES "${release_regex}")
					set(plugin_var "${plugin_var}_RELEASE")
				endif()
				if(CMAKE_CONFIGURATION_TYPES MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "Debug")
					set(plugin_var "${plugin_var}_DEBUG")
				endif()
			else()
				set(plugin_var "${plugin_var}_RELEASE")
			endif()

			set(plugin_path "${${plugin_var}}")
			if(NOT EXISTS "${plugin_path}" AND NOT qtplugins_default)
				message(FATAL_ERROR "${plugin} plugin requested but ${plugin_var} does not exist")
			endif()
		else()
			set(plugin_path "${plugin}")
		endif()

		if(EXISTS "${plugin_path}")
			list(APPEND qtplugins_paths "${plugin_path}")
		endif()
	endforeach()

	install(CODE
		" INCLUDE( \"${CMAKE_ROOT}/Modules/DeployQt4.cmake\" )
		SET( BU_CHMOD_BUNDLE_ITEMS TRUE )
		FIXUP_QT4_BUNDLE( \"\${CMAKE_INSTALL_PREFIX}/${executable}\" \"${qtplugins_paths}\" \"${libs}\" \"${dirs}\" \"${plugins_dir}\" ) "
	)
endfunction(install_qt4_app)
