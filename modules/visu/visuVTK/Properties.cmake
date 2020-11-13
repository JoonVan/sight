
set( NAME visuVTK )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    fwCore
    fwRender
    fwRenderVTK
    fwRuntime
    fwServices
)
set( REQUIREMENTS
    fwData
    servicesReg
)

if(UNIX)
    set( PLUGINS
        xcbglintegrations)
endif()
