
set( NAME visuVTKQt )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    fwCore
    fwData
    fwGui
    fwGuiQt
    fwRenderVTK
    fwRuntime
    fwServices
    fwTools
    fwCom
)
set( REQUIREMENTS
    visuVTK
    servicesReg
)
set( USE_PCH_FROM_TARGET fwRenderVTK )
set( START_BEFORE guiQt )
set( CONAN_DEPS
    ${CONAN_QT}
)