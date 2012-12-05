import QtQuick 2.0
import Pug 1.0

import "js/identify.js" as Identify

CookQueue {
    id: self
    details: input.details
    property var input
    property var metadata: {} 
    count: input !== null ? input.details.length : 0

    inputs: Input { name: "input" }
    params: Param { name: "metadata" }

    component: Component {
        Script {
            property int index
            script: "identify"
            
            property string format: Identify.buildFormatString(self.metadata)
            property string path: input.details[index].element.paths[0]

            params: [
                Param { name: "format";
                        property string flagPrefix: "-" },
                Param { name: "path";
                        property bool arg: true }
            ]

            onCooked: {
                debug("Identify.onCooked " + status);
                if (status == Operation.Finished) {
                    var lines = stdout.replace(/\n$/, "").split("\n");
                    var data = {};
                    debug("lines is " + JSON.stringify(lines));
                    for (var i = 0; i < lines.length; i++) {
                        var tokens = lines[i].split("=");
                        debug("tokens is " + JSON.stringify(tokens));
                        data[tokens[0]] = tokens[1];
                    }
                    self.setDetail(index, "identify", data);
                }
            }
            
            onCook: {
                debug("onCook");
            }
        }
    }
}        
