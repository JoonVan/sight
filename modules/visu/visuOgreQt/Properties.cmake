
set( NAME visuOgreQt )
set( VERSION 0.1 )
set( TYPE MODULE )
set( START ON )
set( DEPENDENCIES
    fwCore
    fwCom
    fwData
    fwGui
    fwGuiQt
    fwRenderOgre
    fwRuntime
)
set( REQUIREMENTS
    visuOgre
    servicesReg
)
set( CONAN_DEPS
    ${CONAN_QT}
)
set( WARNINGS_AS_ERRORS ON )

if(UNIX)
    set(PLUGINS xcbglintegrations)
endif()
