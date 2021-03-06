set( NAME Tuto02DataServiceBasic )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations
    guiQt                   # Start the module, load qt implementation of gui

    # Objects declaration
    fwData
    servicesReg             # fwService

    # UI declaration/Actions
    gui

    # Reader
    ioVTK                   # Contains the reader and writer for VTK files (image and mesh).

    # Services
    visuBasic               # Loads basic rendering services for images and meshes.
)

moduleParam(
        appXml
    PARAM_LIST
        config
    PARAM_VALUES
        TutoDataServiceBasic_AppCfg
) # Main application's configuration to launch
