set( NAME fwRuntime )
set( VERSION 0.3 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwCore fwTools)
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET fwCore )
set( WARNINGS_AS_ERRORS ON)
set( GENERATE_OBJECT_LIB ON) # Needed for fwRuntimeDetailTest (generates fwRuntime_obj target)

set( CONAN_DEPS
    ${CONAN_BOOST}
)

if(WIN32)
    list(APPEND CONAN_DEPS ${CONAN_WINICONV})
    list(APPEND CONAN_DEPS ${CONAN_LIBXML2})
endif()
