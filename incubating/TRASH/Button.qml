// Button.qml
// 9/22/2012 jichi

import QtQuick 2.0

Item {
  property alias text: textItem.text
  property alias color: textItem.color

  property bool operator: false

  signal clicked

  width: 30
  height: 50

  Text {
    id: textItem
    font.pixelSize: 48
    wrapMode: Text.WordWrap
    lineHeight: 0.75
    color: "white"
  }

//  Rectangle {
//    color: "red"
//    opacity: 0.2
//    anchors.fill: mouse
//  }

  MouseArea {
    id: mouse
    anchors.fill: parent
    anchors.margins: -5
    onClicked: {
      //parent.clicked()
      if (operator)
        window.operatorPressed(parent.text)
      else
        window.digitPressed(parent.text)
    }
  }
}

// EOF
