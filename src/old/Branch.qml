import QtQuick 2.0
import Pug 1.0

import "3rdparty/js/sprintf.js" as Sprintf
import "js/branch.js" as Branch

Node {
    id: self
    property Node input
    property string path
    property string pattern
    property bool exactMatch
    property bool constraints
    property var __fieldNames
    property var __fields 
    property var __parseRegexp
    property var __mapFormat
    property var __downstream
    property var __isBranch: true // sentinel, since we don't have access to QML type info

    UpdateOperation.updateable: true

    property string qmlPath: "pug/qml"

    Property {
        objectName: "input"
        input: true
    }
    
    Property {
        objectName: "pattern"
    }

    Property {
        objectName: "exactMatch"
    }

    Property {
        objectName: "constraints"
    }
    
    Property {
        objectName: "path"
        output: true
    }

    function parse(path) {
        return Branch.parse.call(self, path);
    }
     
    function map(dict) {
        return Branch.map.call(self, dict);
    }

    function mapPattern(pattern, dict) {
        return Branch.mapPattern.call(self, pattern, dict);
    }
    
    function init() {
        return Branch.init.call(self);
    }
    
    function getField(fieldName) {
        return Branch.getField.call(self, fieldName);
    }
    
    function mkpath(env) {
        return Branch.mkpath.call(self, env);
    }
    
    UpdateOperation.onUpdate: {
        Branch.onUpdate.call(self, env);
    }
}