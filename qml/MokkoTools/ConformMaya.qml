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

    property var __releasedTextures: textures.details.map(function (detail) {
        // call getReleasePath on each element path in the detail
        return textures.ReleaseOperation.getReleasePath(detail.element.path);
    })

    stdin: "
        import pymel.core as pm
        print 'OK'
        pm.quit() 
    "

    argv: [
        "mayapy",
        "-"
    ]
    
    onCookedAtIndex: {
        debug("onCooked");
        // var newElement = Util.createElement(self, {pattern: __outputPath});
        // setDetail(index, "element", newElement);
        // setDetail(index, "env", input.details[index].env);
        // debug("new element " + newElement.pattern);
    }
}        
