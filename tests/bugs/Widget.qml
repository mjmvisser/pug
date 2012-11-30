import QtQuick 2.0

Rectangle {
    width: 100
    height: 100
    property alias text: textWidget.text
    
    signal updateText(var t)
    signal updated
        
    onUpdateText: {
        textWidget.text = t.text;
        updated();
    }
    
    Text {
        id: textWidget
    }
}
