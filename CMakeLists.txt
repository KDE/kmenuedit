project(kmenuedit)
set(PROJECT_VERSION "5.22.80")

cmake_minimum_required(VERSION 3.16)

set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.82")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} )

include(FeatureSummary)
include(ECMAddAppIcon)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)
include(KDEClangFormat)
include(KDEGitCommitHooks)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Xml
)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    XmlGui
    DBusAddons
    IconThemes
    KIO
    ItemViews
    Sonnet
    DocTools
    GlobalAccel
)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x054200)

#add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY)

add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_FROM_BYTEARRAY)

add_subdirectory( pixmaps ) 
add_subdirectory( doc )
add_subdirectory(kconf_update)

set(kmenueditcommon_STAT_SRCS preferencesdlg.cpp klinespellchecking.cpp basictab.cpp treeview.cpp kmenuedit.cpp menufile.cpp menuinfo.cpp configurationmanager.cpp globalaccel.cpp)
########### next target ###############
qt_add_dbus_adaptor( kmenueditcommon_STAT_SRCS org.kde.kmenuedit.xml kmenuedit.h KMenuEdit)

ecm_qt_declare_logging_category(kmenueditcommon_STAT_SRCS HEADER kmenuedit_debug.h IDENTIFIER KMENUEDIT_LOG CATEGORY_NAME org.kde.kmenuedit DESCRIPTION "kmenuedit" EXPORT KMENUEDIT)

set(kmenuedit_KDEINIT_SRCS main.cpp ${kmenueditcommon_STAT_SRCS})

add_executable( kmenuedit ${kmenuedit_KDEINIT_SRCS})
target_compile_definitions(kmenuedit PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(kmenuedit
    Qt::DBus
    Qt::Xml
    KF5::DBusAddons
    KF5::GlobalAccel
    KF5::I18n
    KF5::IconThemes
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::ItemViews
    KF5::SonnetCore
    KF5::SonnetUi
    KF5::XmlGui
)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
install(TARGETS kmenuedit ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( PROGRAMS org.kde.kmenuedit.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES kmenueditui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kmenuedit )

ecm_install_icons( ICONS 16-apps-kmenuedit.png  22-apps-kmenuedit.png  32-apps-kmenuedit.png  48-apps-kmenuedit.png DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor  )
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

ecm_qt_install_logging_categories(
        EXPORT KMENUEDIT
        FILE kmenuedit.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )
