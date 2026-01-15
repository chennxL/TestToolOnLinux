#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "psiwrapper::psisrv" for configuration "Release"
set_property(TARGET psiwrapper::psisrv APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(psiwrapper::psisrv PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/psisrv"
  )

list(APPEND _IMPORT_CHECK_TARGETS psiwrapper::psisrv )
list(APPEND _IMPORT_CHECK_FILES_FOR_psiwrapper::psisrv "${_IMPORT_PREFIX}/bin/psisrv" )

# Import target "psiwrapper::psiwrapper" for configuration "Release"
set_property(TARGET psiwrapper::psiwrapper APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(psiwrapper::psiwrapper PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libpsiwrapper.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libpsiwrapper.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS psiwrapper::psiwrapper )
list(APPEND _IMPORT_CHECK_FILES_FOR_psiwrapper::psiwrapper "${_IMPORT_PREFIX}/lib/libpsiwrapper.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
