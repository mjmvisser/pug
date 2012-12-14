import Pug 1.0

import "js/mustache.js" as Mustache

Process {
    id: self
    count: scene !== null ? scene.details.length : 0
    property var scene
    property var refs

    inputs: [
        Input { name: "scene" },
        Input { name: "refs" }
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

    property string __currentScene: scene.details[index].element.path
    
    function __refPaths() {
        var result = [];
        for (var i = 0; i < refs.details.length; i++) {
            result[i] = refs.details[i].element.path;
        }
        return result;
    }
    
    stdin: Mustache.render(dedent('
        from __future__ import print_function
        import os, sys
        if "MOKKO_WORK_TYPE" in os.environ:
            if "MOKKO_DEPARTMENT" in os.environ and os.environ["MOKKO_WORK_TYPE"] == "asset":
                if os.environ["MOKKO_DEPARTMENT"] == "shaded":
                    from mokmaya.scenefile import ShadedAssetConformer as Conformer
                elif os.environ["MOKKO_DEPARTMENT"] == "rig" and os.environ["MOKKO_ASSET_TYPE"] == "lightset":
                    from mokmaya.scenefile import LightsetAssetConformer as Conformer
                elif "MOKKO_DEPARTMENT" in os.environ and os.environ["MOKKO_DEPARTMENT"] == "rig":
                    from mokmaya.scenefile import RigAssetConformer as Conformer
                else: 
                    from mokmaya.scenefile import AssetConformer as Conformer
            elif os.environ["MOKKO_WORK_TYPE"] == "shot":
                from mokmaya.scenefile import ShotConformer as Conformer
            else:
                print("Don\'t know how to conform work type {$1}".format(os.environ["MOKKO_WORK_TYPE"]), file=sys.stderr)
                os._exit(1)
        else:
            print("MOKKO_WORK_TYPE is not set.", file=sys.stderr)
            os._exit(1)

        import pymel.core as pm
        pm.openFile("{{{scene}}}", loadReferenceDepth="none", force=True)
        
        print("files to conform: ", [{{#refsDetails}}"{{{element.pattern}}}", {{/refsDetails}}], file=sys.stderr)
        
        conformer = Conformer()
        conformer.conform()
        
        pm.saveAs("{output}", force=True)
        
        os._exit(0)
    '), {"scene": scene.details[index].element.path,
         "refsDetails": refs.details});

    argv: [
        "mayapy",
        "-"
    ]
    
    onCookedAtIndex: {
        if (status === Operation.Finished) {
            debug("onCooked");
            debug("ReleaseOperation.details: " + refs.ReleaseOperation.details);
            for (var refIndex = 0; refIndex < refs.details.length; refIndex++) {
                debug("conforming " + refs.details[refIndex].element.pattern);
                debug(" -> " + refs.ReleaseOperation.details[refIndex].element.pattern);            
            }
        }
    }
}        
