import Pug 1.0

Node {
    id: self
    property list<Node> sources
    
    inputs: Input { name: "sources" }

    signal update(var context)
    signal updateFinished(int status)

    signal cook(var context)
    signal cookFinished(int status)
    
    function merge() {
        for (var j = 0; j < sources.length; j++) {
            if (sources[j]) {
                for (var index = 0; index < sources[j].details.length; index++) {
                    details.push(sources[j].details[index]);
                }
            }
        }
        count = details.length;
        detailsChanged();
    }
    
    onUpdate: {
        merge();
        updateFinished(Operation.Finished);
    }
    
    onCook: {
        merge();
        cookFinished(Operation.Finished);
    }
}
