# Doxygen support for scsl.

add_custom_target(manpages)

macro(md2man source)
	block()
		set(SOURCE_MANPAGE)
		set(SOURCE_SECTION)
		string(REGEX REPLACE "^.+/([^/]+)\.md$" "\\1" SOURCE_MANPAGE ${source})
		string(REGEX REPLACE "^.+/[^/]+\.([0-9])\.md$" "\\1" SOURCE_SECTION ${source})
		file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man/man${SOURCE_SECTION})
		configure_file(${source} ${SOURCE_MANPAGE}.scdoc)
		add_custom_command(TARGET manpages
			COMMAND scdoc < ${SOURCE_MANPAGE}.scdoc > man/man${SOURCE_SECTION}/${SOURCE_MANPAGE})
	endblock()
endmacro()

md2man(docs/emsha.3.md)

find_package(Doxygen)
if (${DOXYGEN_FOUND})
	# prefer scdocs for manpages.
	set(DOXYGEN_GENERATE_MAN NO)
	set(DOXYGEN_GENERATE_LATEX YES)
	set(DOXYGEN_EXTRACT_ALL YES)
	set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/docs/mainpage.md")
	set(DOXYGEN_EXCLUDE_PATTERNS "test_*" "*.cc" )
	message(STATUS "Doxygen found, building docs.")

	doxygen_add_docs(${PROJECT_NAME}_docs
		${HEADER_FILES}
		ALL
		USE_STAMP_FILE)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
		${CMAKE_CURRENT_BINARY_DIR}/latex
		DESTINATION share/doc/${PROJECT_NAME}/doxygen)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man
		DESTINATION share)
	add_dependencies(${PROJECT_NAME}_docs manpages)
endif ()

