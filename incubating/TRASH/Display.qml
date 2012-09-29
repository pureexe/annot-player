// Display.qml
// 9/22/2012 jichi

import QtQuick 2.0

Item {
  id: display

  function displayOperator(operator)
  {
    listView.model.append({ "operator": operator, "operand": "" })
  }

  function newLine(operator, operand)
  {
    listView.model.append({ "operator": operator, "operand": operand })
  }

  function appendDigit(digit)
  {
    if (!listView.model.count)
      listView.model.append({ "operator": "", "operand": "" })
    var i = listView.model.count - 1;
    listView.model.get(i).operand = listView.model.get(i).operand + digit;
  }

  Item {
    id: theItem
    width: parent.width + 32
    height: parent.height

    Rectangle {
      id: rect
      x: 16
      color: "white"
      height: parent.height
      width: display.width - 16
    }
    Image {
      anchors.right: rect.left
      source: "images/paper-edge-left.png"
      height: parent.height
      fillMode: Image.TileVertically
    }
    Image {
      anchors.left: rect.right
      source: "images/paper-edge-right.png"
      height: parent.height
      fillMode: Image.TileVertically
    }

    Image {
      source: "images/paper-grip.png"
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: 20
    }

    ListView {
      id: listView
      x: 16; y: 30
      width: display.width
      height: display.height
      delegate: Item {
        height: 20
        width: parent.width
        Text {
          id: operator
          x: 8
          font.pixelSize: 18
          color: "#6da43d"
          text: model.operator
        }
        Text {
          id: operand
          font.pixelSize: 18
          anchors.right: parent.right
          anchors.rightMargin: 26
          text: model.operand
        }
      }
      model: ListModel { }
    }

  }

}

// EOF
