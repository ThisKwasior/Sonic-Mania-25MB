#----------------------------------------------------------------
# Generated CMake target import file for configuration "minsizerel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Ogg::ogg" for configuration "minsizerel"
set_property(TARGET Ogg::ogg APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(Ogg::ogg PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "C"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/libogg.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Ogg::ogg )
list(APPEND _IMPORT_CHECK_FILES_FOR_Ogg::ogg "${_IMPORT_PREFIX}/lib/libogg.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
