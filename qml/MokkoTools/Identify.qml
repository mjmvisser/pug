import QtQuick 2.0
import Pug 1.0

import "js/identify.js" as Identify

Script {
    id: self
    details: input.details

    property var input
    property var metadata: {} 

    inputs: Input { name: "input" }
    params: Param { name: "metadata" }

    argv: [
        "identify",
        "-format", Identify.buildFormatString(self.metadata),
        input.details[index].element.paths[0]
    ]
    onCookedAtIndex: {
        debug("Identify.onCooked " + status);
        if (status == Operation.Finished) {
            var lines = details[index].process.stdout.replace(/\n$/, "").split("\n");
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
}        
