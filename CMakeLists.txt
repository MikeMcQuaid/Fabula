project(Fabula)
set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION_COUNT 3)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "Building ${PROJECT_NAME} in ${CMAKE_BUILD_TYPE} mode")
cmake_minimum_required(VERSION 2.8.0)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtSQL REQUIRED)
include(UseQt4)

if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(
		-Wall -Werror -Wextra -Wold-style-cast -Wformat=2 -Winit-self
		-Wundef -Wpointer-arith -Wcast-qual -Wconversion -Wunused
		-Wmissing-format-attribute -Wredundant-decls -Winline
		-Wnon-virtual-dtor -Woverloaded-virtual -Wstrict-null-sentinel
		-Wno-pmf-conversions -Wctor-dtor-privacy
	)
endif()

add_definitions( -PROJECT_VERSION=\"${PROJECT_VERSION}\")

set(SOURCES
	main.cpp
	MainWindow.cpp
	Database.cpp
	TableTreeModel.cpp
	PreferencesDialog.cpp
	EventDialog.cpp
	EventDelegate.cpp
)
qt4_automoc(${SOURCES})

set(HEADERS
	MainWindow.h
	Database.h
	TableTreeModel.h
	PreferencesDialog.h
	EventDialog.h
	EventDelegate.h
)
qt4_wrap_cpp(MOC_SOURCES ${HEADERS})

set(FORMS
	MainWindow.ui
	PreferencesDialog.ui
	EventDialog.ui
)
qt4_wrap_ui(FORMS_SOURCES ${FORMS})

set(RESOURCES Fabula.qrc)
QT4_ADD_RESOURCES(RESOURCES_SOURCES ${RESOURCES})

if(APPLE)
	set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
	set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
	set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
	set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
	set(MACOSX_BUNDLE_COPYRIGHT "2010 Mike McQuaid")
	set(MACOSX_BUNDLE_ICON_FILE "audio-input-microphone.icns")
	set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.mikemcquaid")
	set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")

	set(MACOSX_BUNDLE_RESOURCES "${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources")
	set(MACOSX_BUNDLE_ICON "${${PROJECT_NAME}_SOURCE_DIR}/icons/${MACOSX_BUNDLE_ICON_FILE}")
	execute_process(
		COMMAND	${CMAKE_COMMAND} -E make_directory ${MACOSX_BUNDLE_RESOURCES}
	)
	execute_process(
		COMMAND	${CMAKE_COMMAND} -E copy_if_different ${MACOSX_BUNDLE_ICON} ${MACOSX_BUNDLE_RESOURCES}
	)
endif()

include_directories(${${PROJECT_NAME}_BINARY_DIR})

add_executable(${PROJECT_NAME}
	WIN32 MACOSX_BUNDLE
	${SOURCES} ${MOC_SOURCES} ${FORMS_SOURCES} ${RESOURCES_SOURCES}
)
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})

if(UNIX AND NOT APPLE)
	string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)
	SET(BIN_INSTALL_DIR "bin")
	SET(DOC_INSTALL_DIR "share/doc/${PROJECT_NAME_LOWERCASE}/") 
else()
	SET(BIN_INSTALL_DIR ".")
	SET(DOC_INSTALL_DIR ".")
endif()

message(STATUS "${PROJECT_NAME} will be installed to ${CMAKE_INSTALL_PREFIX}")
install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_DIR})
install(FILES README.txt LICENSE.txt DESTINATION ${DOC_INSTALL_DIR})
