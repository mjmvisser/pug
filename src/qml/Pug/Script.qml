import Pug 1.0

import "js/script.js" as Script

Process {
    id: self
    property string script
    
    argv: Script.buildArgv.call(self)
    
    function generatePath(index, hasFrames) {
        return Script.generatePath.call(self, index, hasFrames);
    }
}