import QtQuick 2.0
import Pug 1.0

Node {
    id: self
    property string pattern
    property var elements: []
    
    params: Param { name: "pattern" }
    
    signal update(var env)
    signal updated(int status)
    
    onUpdate: {
        debug(".onUpdate");
        elements = [];
        var newElement = Util.createElement(self, {pattern: self.pattern});
        newElement.scan();
        elements.push(newElement);
        elementsChanged();  // force any bindings to update
        debug("added " + newElement.pattern);
        debug("elements length is now " + elements.length);
        updated(Operation.Finished);
    }
}
