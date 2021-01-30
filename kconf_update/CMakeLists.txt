add_executable(kmenuedit_globalaccel globalaccel.cpp ../globalaccel.cpp)

target_link_libraries(kmenuedit_globalaccel Qt::DBus KF5::ConfigCore KF5::GlobalAccel KF5::I18n KF5::Service)

install(TARGETS kmenuedit_globalaccel DESTINATION ${KDE_INSTALL_LIBDIR}/kconf_update_bin)
