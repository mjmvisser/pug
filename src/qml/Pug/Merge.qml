import Pug 1.0

Node {
    id: self
    property list<Node> sources
    
    inputs: Input { name: "sources" }

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
    
    UpdateOperation.onCook: {
        merge();
        UpdateOperation.cookFinished(Operation.Finished);
    }
    
    CookOperation.onCook: {
        merge();
        CookOperation.cookFinished(Operation.Finished);
    }
}
