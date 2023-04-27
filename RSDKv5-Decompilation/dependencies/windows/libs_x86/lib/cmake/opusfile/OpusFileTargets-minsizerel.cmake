#----------------------------------------------------------------
# Generated CMake target import file for configuration "minsizerel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "OpusFile::opusfile" for configuration "minsizerel"
set_property(TARGET OpusFile::opusfile APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(OpusFile::opusfile PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "C"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/libopusfile.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS OpusFile::opusfile )
list(APPEND _IMPORT_CHECK_FILES_FOR_OpusFile::opusfile "${_IMPORT_PREFIX}/lib/libopusfile.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
