import QtQuick 2.0

Item {
  BorderImage {
    source: "shadow-d2.png"

    anchors {
      fill: parent
      leftMargin: -border.left; topMargin: -border.top
      rightMargin: -border.right; bottomMargin: -border.bottom
    }

    border.left: 5; border.top: 4
    border.right: 6; border.bottom: 8
  }
}
