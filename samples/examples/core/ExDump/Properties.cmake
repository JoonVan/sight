set( NAME ExDump )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    guiQt                   # Start the module, allow dark theme

    # Objects declaration
    fwData
    servicesReg             # fwService
    fwMedData

    # UI declaration/Actions
    gui
    style
    flatIcon

    # Reader
    ioAtoms
    ioITK
    ioVTK
    ioVtkGdcm
    ioData

    # Services
    memory
    uiIO
    uiMedDataQt
    2DVisualizationActivity
    3DVisualizationActivity
    ioActivity
    patchMedicalData
    filterUnknownSeries
    monitor
    monitorQt
)

moduleParam(
        memory
    PARAM_LIST
        loading_mode
    PARAM_VALUES
        lazy
) # Set the reading mode to "lazy", so the buffers will only be read in memory when they are used.

moduleParam(guiQt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via guiQt

moduleParam(
        appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExDump_AppCfg
) # Main application's configuration to launch
