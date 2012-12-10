import Pug 1.0

Process {
    id: self
    count: scene !== null ? scene.details.length : 0
    property var scene
    property var textures

    inputs: [
        Input { name: "scene" },
        Input { name: "textures" }
    ]

    function dedent(s) {
        var lines = s.split("\n");
        if (lines[0].length == 0)
            lines.shift();
        var spaceRegexp = RegExp("^[ ]+");
        var indent = spaceRegexp.exec(lines[0]);
        var dedentRegexp = RegExp("^" + indent);
        for (var i = 0; i < lines.length; i++) {
            lines[i] = lines[i].replace(dedentRegexp, "");
        }
        return lines.join("\n");
    }

    stdin: dedent("
        import pymel.core as pm
    ");

    argv: [
        "mayapy",
        "-"
    ]
    
    onCookedAtIndex: {
        debug("onCooked");
        debug("ReleaseOperation.details: " + textures.ReleaseOperation.details);
        for (var textureIndex = 0; textureIndex < textures.details.length; textureIndex++) {
            debug("conforming " + textures.details[textureIndex].element.pattern);
            debug(" -> " + textures.ReleaseOperation.details[textureIndex].element.pattern);            
        }
    }
}        
