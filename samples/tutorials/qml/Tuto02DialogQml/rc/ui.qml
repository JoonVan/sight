import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3
import Tuto02DialogQml 1.0

import styleQml 1.0

ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true
    modality: Qt.NonModal

    title: qsTr("Tuto02DialogQml 0.1")

    // the name of each section
    header:         TabBar {
        id: bar
        width: parent.width
        TabButton {
            text: qsTr("Input")
        }
        TabButton {
            text: qsTr("Location")
        }
        TabButton {
            text: qsTr("Logger")
        }
        TabButton {
            text: qsTr("Message")
        }
        TabButton {
            text: qsTr("MultiSelector")
        }
        TabButton {
            text: qsTr("Progress")
        }
        TabButton {
            text: qsTr("PulseProgress")
        }
        TabButton {
            text: qsTr("Selector")
        }
    }

    // the page of each section associated to the menu TabBar
    StackLayout {
        anchors.fill: parent
        currentIndex: bar.currentIndex
        InputDialogs { }
        LocationDialogs { }
        LoggerDialogs { }
        MessageDialogs { }
        MultiSelectorDialogs { }
        ProgressDialogs { }
        PulseProgressDialogs { }
        SelectorDialogs { }
    }

    // this toolbar is for the progressDialog to fill this rectangle
    footer: ToolBar {
        objectName: "fwGuiQml_ProgressBar"
        background:  Rectangle {
            implicitHeight: 40
            color: Material.background

            Rectangle {
                width: parent.width
                height: 1
                anchors.bottom: parent.bottom
                color: "transparent"
                border.color: Material.accent
            }
        }
    }
    // Set the global theme inside the singleton "Theme" and then set the Window theme via the Singleton
    // This singleton will permit to set the same theme in all window open for this application
    Component.onCompleted: {
        Theme.accent = Material.color(Material.LightBlue)
        Theme.theme = Material.Light
        Theme.foreground = (Material.theme == Material.Dark) ? Material.color(Material.BlueGrey, Material.Shade100) : Material.color(Material.BlueGrey, Material.Shade900)
        Theme.background = Material.background
        Theme.primary = Material.color(Material.Teal)
        Theme.elevation = Material.elevation
        Material.accent = Theme.accent
        Material.theme = Theme.theme
        Material.foreground = Theme.foreground
        Material.background = Theme.background
        Material.primary = Theme.primary
        Material.elevation = Theme.elevation
    }
}
