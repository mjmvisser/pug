import QtQuick 2.0
import Pug 1.0

Node {
    id: self
    property string pattern
    
    params: Param { name: "pattern" }
    
    signal update(var env)
    signal updated(int status)
    
    onUpdate: {
        debug(".onUpdate");
        var newElement = Util.createElement(self, {pattern: self.pattern});
        newElement.scan();
        var details = [];
        for (var i = 0; i < self.details.length; i++) {
            details.push(self.details[i]);
        }
        details.push({"element": newElement, "env": {}});
        self.details = details;
        debug("added " + newElement.pattern);
        debug("details length is now " + self.details.length);
        debug(self.details[0].element.pattern);
        updated(Operation.Finished);
    }
}
