set( NAME fwServices )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwActivities fwCom fwDataCamp fwRuntime )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )
set( WARNINGS_AS_ERRORS ON )

set( CONAN_DEPS
    ${CONAN_BOOST}
)