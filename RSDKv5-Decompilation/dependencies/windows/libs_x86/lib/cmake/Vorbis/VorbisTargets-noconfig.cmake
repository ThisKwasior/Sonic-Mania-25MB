#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Vorbis::vorbis" for configuration ""
set_property(TARGET Vorbis::vorbis APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(Vorbis::vorbis PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libvorbis.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Vorbis::vorbis )
list(APPEND _IMPORT_CHECK_FILES_FOR_Vorbis::vorbis "${_IMPORT_PREFIX}/lib/libvorbis.a" )

# Import target "Vorbis::vorbisenc" for configuration ""
set_property(TARGET Vorbis::vorbisenc APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(Vorbis::vorbisenc PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libvorbisenc.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Vorbis::vorbisenc )
list(APPEND _IMPORT_CHECK_FILES_FOR_Vorbis::vorbisenc "${_IMPORT_PREFIX}/lib/libvorbisenc.a" )

# Import target "Vorbis::vorbisfile" for configuration ""
set_property(TARGET Vorbis::vorbisfile APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(Vorbis::vorbisfile PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libvorbisfile.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Vorbis::vorbisfile )
list(APPEND _IMPORT_CHECK_FILES_FOR_Vorbis::vorbisfile "${_IMPORT_PREFIX}/lib/libvorbisfile.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
