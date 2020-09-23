
set( NAME ExSolvePnP )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml
    arData
    ctrlCamp
    fwData
    fwlauncher
    gui
    guiQt
    ioCalibration
    ioVTK
    uiPreferences
    uiTools
    preferences
    servicesReg
    registrationCV
    videoQt
    videoTools
    visuVTK
    visuVTKAdaptor
    visuVTKARAdaptor
    visuVTKQt
    ctrlPicking
    media
    )

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExSolvePnPConfig)
