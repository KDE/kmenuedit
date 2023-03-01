add_executable(kmenuedit_globalaccel globalaccel.cpp ../globalaccel.cpp)

target_link_libraries(kmenuedit_globalaccel Qt::DBus KF6::ConfigCore KF6::GlobalAccel KF6::I18n KF6::Service)

install(TARGETS kmenuedit_globalaccel DESTINATION ${KDE_INSTALL_LIBDIR}/kconf_update_bin)
