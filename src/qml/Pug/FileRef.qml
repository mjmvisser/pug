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
        self.details.push({"element": newElement, "env": {}});
        self.detailsChanged();
        debug("added " + newElement.pattern);
        debug("details length is now " + self.details.length);
        updated(Operation.Finished);
    }
}
