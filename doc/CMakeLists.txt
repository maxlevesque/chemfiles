find_package(Sphinx REQUIRED)
find_package(Doxygen REQUIRED)

configure_file(${PROJECT_SOURCE_DIR}/Doxyfile ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/doc/conf.cmake.py ${PROJECT_BINARY_DIR}/doc/conf.py)

file(GLOB_RECURSE ALL_HEADERS ${PROJECT_SOURCE_DIR}/include/**.h*)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doxygen/xml/index.xml
    COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
    DEPENDS ${ALL_HEADERS}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
)

add_custom_target(doxygen ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doxygen/xml/index.xml
)

set(SPHINX_CONF_DIR ${PROJECT_BINARY_DIR}/doc)
set(SPHINX_INPUT_DIR ${PROJECT_SOURCE_DIR}/doc)
set(SPHINX_DESTINATION ${PROJECT_BINARY_DIR}/doc)
set(SPHINX_DEPENDENCIES doxygen)

set(sphinx_target_base_name doc)
Sphinx_add_targets(${sphinx_target_base_name} ${SPHINX_CONF_DIR} ${SPHINX_INPUT_DIR} ${SPHINX_DESTINATION} ${SPHINX_DEPENDENCIES})

if( ${SPHINX_HTML_OUTPUT} )
    install( DIRECTORY ${SPHINX_DESTINATION}/html
        DESTINATION share/chemfiles/doc
        COMPONENT Doc
        PATTERN "${SPHINX_DESTINATION}/html/*"
        )
endif()

if(${SPHINX_LATEX_OUTPUT})
    set(DOC_WITH_LOCAL_DOXYGEN OFF)

    # Build the PDF with pdflatex
    find_package(LATEX)
    if( NOT PDFLATEX_COMPILER )
      message("pdflatex compiler was not found. Please pass to advanced mode and provide its full path")
    else()
      # Needs to be executed twice to get table of contents.
      add_custom_command( TARGET ${sphinx_target_base_name}_latex
        POST_BUILD
        COMMAND ${PDFLATEX_COMPILER}
          ${SPHINX_DESTINATION}/latex/chemfiles.tex
          -output-directory ${SPHINX_DESTINATION}/latex
        COMMAND ${MAKEINDEX_COMPILER}
            chemfiles.idx
        COMMAND ${PDFLATEX_COMPILER}
          ${SPHINX_DESTINATION}/latex/chemfiles.tex
          -output-directory ${SPHINX_DESTINATION}/latex
        WORKING_DIRECTORY ${SPHINX_DESTINATION}/latex
        COMMENT "Building PDF"
        )
  endif()
endif()
