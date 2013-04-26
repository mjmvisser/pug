import Pug 1.0

Node {
    id: self
    property Node input

    inputs: Input { name: "input" }

    count: input.count

    details: nodeC.details

    Node {
        id: nodeB
        name: "nodeB"

        property Node input: self.input
        
        inputs: Input { name: "input" }
        
        count: self.count
        
        CookOperation.onCook: {
            for (index = 0; index < count; index++) {
                details[index] = {"result": input.details[index].result + " -> " + name + index,
                                  "context": context};
            }
            detailsChanged();
            CookOperation.cookFinished(Operation.Finished);                              
        }
    }

    Node {
        id: nodeC
        name: "nodeC"
        property Node input: nodeB
        output: true
        
        inputs: Input { name: "input" }

        count: input.count
        
        CookOperation.onCook: {
            for (index = 0; index < count; index++) {
                details[index] = {"result": input.details[index].result + " -> " + name + index,
                                  "context": context};
            }
            detailsChanged();
            CookOperation.cookFinished(Operation.Finished);                              
        }
    }
}        
