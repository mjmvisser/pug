import QtQuick 2.0
import Pug 1.0

Script {
    id: renderLowUnd
    script: "genundist2"

    // inputs
    property File nukeNodeFile
    property File plateFile

    Property {
        name: "nukeNodeFile"
        input: true
    }
    
    Property {
        name: "plateFile"
        input: true
    }
    
    // parameters
    property string nukeNodePath: nukeNodeFile.paths
    property string platePath: plateFile.paths
    property string format
    property var paths
    
    Property {
        name: "nukeNodePath"
    }

    Property {
        name: "platePath"
    }
    
    Property {
        name: "format"
    }
    
    Property {
        name: "paths"
        output: true
    }
}
