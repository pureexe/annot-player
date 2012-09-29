// NumberPad.qml
// 9/22/2012 jichi

import QtQuick 2.0

Grid {
  columns: 3
  columnSpacing: 32
  rowSpacing: 16

  Button { text: "7" }
  Button { text: "8" }
  Button { text: "9" }
  Button { text: "4" }
  Button { text: "5" }
  Button { text: "6" }
  Button { text: "1" }
  Button { text: "2" }
  Button { text: "3" }
  Button { text: "0" }
  Button { text: "." }
  Button { text: " " }
  Button { text: "±"; color: "#6da43d"; operator: true }
  Button { text: "−"; color: "#6da43d"; operator: true }
  Button { text: "+"; color: "#6da43d"; operator: true }
  Button { text: " "; color: "#6da43d"; operator: true }
  Button { text: "÷"; color: "#6da43d"; operator: true }
  Button { text: "×"; color: "#6da43d"; operator: true }
  Button { text: "C"; color: "#6da43d"; operator: true }
  Button { text: " "; color: "#6da43d"; operator: true }
  Button { text: "="; color: "#6da43d"; operator: true }
}

// EOF
