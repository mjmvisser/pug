import QtQuick 2.0

FancyWidget {
    signal fancyUpdateText(var t)
    
    Component.onCompleted: fancyUpdateText.connect(updateText)
}
