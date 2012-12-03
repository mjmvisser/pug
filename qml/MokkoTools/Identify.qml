import QtQuick 2.0
import Pug 1.0

import "js/identify.js" as Identify

CookQueue {
    id: self
    property var input
    property var elements: input.elements
    property var metadata: {} 
    count: input !== null ? input.elements.length : 0

    inputs: Input { name: "input" }
    params: Param { name: "metadata" }

    component: Component {
        Script {
            property int index
            script: "identify"
            property string format: Identify.buildFormatString(self.metadata)
            property string path: elements[index].paths[0]

            params: [
                Param { name: "format";
                        property string flagPrefix: "-" },
                Param { name: "path";
                        property bool noFlag: true }
            ]

            onCooked: {
                debug("Identify.onCooked called");
                var lines = stdout.replace(/\n$/, "").split("\n");
                debug("lines is " + JSON.stringify(lines));
                for (var i = 0; i < lines.length; i++) {
                    var tokens = lines[i].split("=");
                    debug("tokens is " + JSON.stringify(tokens));
                    var data = input.elements[index].data;
                    if (typeof data === "undefined")
                        data = {};
                    data[tokens[0]] = tokens[1]; 
                    input.elements[index].data = data;
                }
            }
        }
    }
}        
