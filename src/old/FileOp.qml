import QtQuick 2.0
import Pug 1.0

Script {
    id: self
    script: Qt.resolvedUrl("scripts/fileOp.py").replace("file://", "")
    CookOperation.cookable: true

    property Node src
    property Node dest
    
    property string srcPath: src.path
    property string destPath: dest.path
    property string mode // "copy", "move", "hardlink" or "symlink", TODO: enums?

    Property {
        objectName: "src"
        input: true
    }

    Property {
        objectName: "dest"
        input: true
    }

    Property {
        objectName: "srcPath"
    }
    
    Property {
        objectName: "destPath"
    }
    
    Property {
        objectName: "mode"
    }

    function init() {
        // nothing to do
    }    
}