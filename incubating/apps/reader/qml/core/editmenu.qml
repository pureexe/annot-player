
import QtQuick 2.0

Rectangle {
  id: editMenu
  height: 480; width:1000
  color: "powderblue"
  property color buttonBorderColor: "#7A8182"
  property color buttonFillColor: "#61BDCACD"
  property string menuName:"Edit"

    gradient: Gradient{
            GradientStop { position: 0.0; color: "#6A7570" }
            GradientStop { position: 1.0; color: Qt.darker("#6A7570") }
  }

  Rectangle{
    id:actionContainer
     color:"transparent"
    anchors.centerIn: parent
    width: parent.width; height: parent.height / 5
    Row{
      anchors.centerIn: parent
      spacing: parent.width/9
      Button{
        id: loadButton
        buttonColor: buttonFillColor
        label: "Copy"
        labelSize:16
        borderColor:buttonBorderColor
        height: actionContainer.height
        width: actionContainer.width/6
        onButtonClicked:textArea.copy()
        gradient: Gradient {
          GradientStop { position: 0.0; color: Qt.lighter(buttonFillColor,1.25) }
          GradientStop { position: 0.67; color: Qt.darker(buttonFillColor,1.3) }
        }
      }

      Button{
        id: saveButton
        height: actionContainer.height
        width: actionContainer.width/6
        buttonColor: buttonFillColor
        label: "Paste"
        borderColor:buttonBorderColor
        labelSize:16
        onButtonClicked:textArea.paste()
        gradient: Gradient {
          GradientStop { position: 0.0; color: Qt.lighter(buttonFillColor,1.25) }
          GradientStop { position: 0.67; color: Qt.darker(buttonFillColor,1.3) }
        }
      }
      Button{
        id: exitButton
        label: "Select All"
        height: actionContainer.height
        width: actionContainer.width/6
        labelSize:16
        buttonColor: buttonFillColor
        borderColor:buttonBorderColor
        onButtonClicked:textArea.selectAll()
        gradient: Gradient {
          GradientStop { position: 0.0; color: Qt.lighter(buttonFillColor,1.25) }
          GradientStop { position: 0.67; color: Qt.darker(buttonFillColor,1.3) }
        }
      }
    }
  }

}
