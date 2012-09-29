// librarydialog.qml
// 9/26/2012 jichi

import QtQuick 2.0
import "core"

Rectangle {
  id: screen
  width: 600; height: 400
  property int partition: height/3
  color: "transparent"

  state: "DRAWER_OPEN"

  // - Items -

  // MenuBar on the top portion of the screen
  MenuBar {
    id:menuBar
    height: screen.partition; width: screen.width
    z:1
  }

  // The editable text area
  TextArea {
    id:textArea
    y:drawer.height
    //color: "#3f3f3f"
    color: "transparent"
    //fontColor: "#dcdccc"
    fontColor: "red"
    height: partition*2; width:parent.width
  }
  //Item 3: The drawer handle
  Rectangle{
    id:drawer
    height:15; width: parent.width
    border.color : "#6a6d6a"
    border.width: 1
    z:1
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#8c8f8c" }
        GradientStop { position: 0.17; color: "#6a6d6a" }
        GradientStop { position: 0.77; color: "#3f3f3f" }
        GradientStop { position: 1.0; color: "#6a6d6a" }
      }
    Image{
      id: arrowIcon
      source: "images/arrow.png"
      anchors.horizontalCenter: parent.horizontalCenter

      Behavior{NumberAnimation{property: "rotation";easing.type: Easing.OutExpo }}
    }

    MouseArea{
      id: drawerMouseArea
      anchors.fill:parent
      hoverEnabled: true
      onEntered: parent.border.color = Qt.lighter("#6a6d6a")
      onExited:  parent.border.color = "#6a6d6a"
      onClicked:{
        if (screen.state == "DRAWER_CLOSED")
          screen.state = "DRAWER_OPEN"
        else if (screen.state == "DRAWER_OPEN")
          screen.state = "DRAWER_CLOSED"
      }
    }
  }

  // - Drawer Animation  -

  states: [
    State{
      name: "DRAWER_OPEN"
      PropertyChanges { target: menuBar; y: 0}
      PropertyChanges { target: textArea; y: partition + drawer.height}
      PropertyChanges { target: drawer; y: partition}
      PropertyChanges { target: arrowIcon; rotation: 180}
    },
    State{
      name: "DRAWER_CLOSED"
      PropertyChanges { target: menuBar; y:-height; }
      PropertyChanges { target: textArea; y: drawer.height; height: screen.height - drawer.height}
      PropertyChanges { target: drawer; y: 0}
      PropertyChanges { target: arrowIcon; rotation: 0}
    }
  ]

  transitions: [
    Transition{
      to: "*"
      NumberAnimation { target: textArea; properties: "y, height"; duration: 100; easing.type:Easing.OutExpo }
      NumberAnimation { target: menuBar; properties: "y"; duration: 100;easing.type: Easing.OutExpo }
      NumberAnimation { target: drawer; properties: "y"; duration: 100;easing.type: Easing.OutExpo }
    }
  ]
}
