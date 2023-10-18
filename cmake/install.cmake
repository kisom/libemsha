### Install ###

include(CMakePackageConfigHelpers)

### library packaging for CMake and pkgconfig to find built targets.
write_basic_package_version_file(
	${PROJECT_NAME}Config.cmake
	VERSION ${PACKAGE_VERSION}
	COMPATIBILITY SameMajorVersion
)
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion)
configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)

### set up installation targets.

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
	DESTINATION lib/pkgconfig)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
	      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
	DESTINATION share/${PROJECT_CMAKE_CONFIG_NAME}/cmake)


