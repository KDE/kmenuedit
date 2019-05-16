project(kmenuedit)
set(PROJECT_VERSION "5.15.80")

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.12.0")

find_package(ECM 1.7.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(FeatureSummary)
include(ECMAddAppIcon)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Xml
)

find_package(KF5 REQUIRED COMPONENTS
    I18n
    XmlGui
    DBusAddons
    IconThemes
    KIO
    Sonnet
    DocTools
    Init
    KDELibs4Support
)

find_package(KHotKeysDBusInterface QUIET)


#add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY)

add_definitions(-DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_FROM_BYTEARRAY)

add_subdirectory( pixmaps ) 
add_subdirectory( doc )

set(kmenueditcommon_STAT_SRCS preferencesdlg.cpp klinespellchecking.cpp basictab.cpp treeview.cpp kmenuedit.cpp menufile.cpp menuinfo.cpp configurationmanager.cpp )
########### next target ###############
qt5_add_dbus_adaptor( kmenueditcommon_STAT_SRCS org.kde.kmenuedit.xml kmenuedit.h KMenuEdit)

ecm_qt_declare_logging_category(kmenueditcommon_STAT_SRCS HEADER kmenuedit_debug.h IDENTIFIER KMENUEDIT_LOG CATEGORY_NAME org.kde.kmenuedit)

if(NOT WIN32 AND KHotKeysDBusInterface_FOUND)
qt5_add_dbus_interface(
    kmenueditcommon_STAT_SRCS
    ${KHOTKEYS_DBUS_INTERFACE}
    khotkeys_interface)
set(kmenueditcommon_STAT_SRCS ${kmenueditcommon_STAT_SRCS} khotkeys.cpp)

add_definitions(-DWITH_HOTKEYS)
endif()

set(kmenuedit_KDEINIT_SRCS main.cpp ${kmenueditcommon_STAT_SRCS})

kf5_add_kdeinit_executable( kmenuedit ${kmenuedit_KDEINIT_SRCS})
target_compile_definitions(kdeinit_kmenuedit PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(kdeinit_kmenuedit
    Qt5::DBus
    Qt5::Xml
    KF5::DBusAddons
    KF5::I18n
    KF5::IconThemes
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::SonnetCore
    KF5::SonnetUi
    KF5::XmlGui
    KF5::KDELibs4Support
)

install(TARGETS kdeinit_kmenuedit  DESTINATION ${KDE_INSTALL_LIBDIR} )

target_link_libraries( kmenuedit kdeinit_kmenuedit )
install(TARGETS kmenuedit ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( PROGRAMS org.kde.kmenuedit.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES kmenueditui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kmenuedit )

ecm_install_icons( ICONS 16-apps-kmenuedit.png  22-apps-kmenuedit.png  32-apps-kmenuedit.png  48-apps-kmenuedit.png DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor  )
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

install( FILES kmenuedit.categories DESTINATION ${KDE_INSTALL_CONFDIR} )
