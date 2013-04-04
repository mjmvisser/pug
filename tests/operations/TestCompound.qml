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
        
        signal cookAtIndex(int index, var context)
        signal cookAtIndexFinished(int index, int status)

        onCookAtIndex: {
            details[index] = {"result": input.details[index].result + " -> " + name + index,
                              "context": context};
            detailsChanged();
            cookAtIndexFinished(index, Operation.Finished);                              
        }
    }

    Node {
        id: nodeC
        name: "nodeC"
        property Node input: nodeB
        output: true
        
        inputs: Input { name: "input" }

        count: input.count
        
        signal cookAtIndex(int index, var context)
        signal cookAtIndexFinished(int index, int status)

        onCookAtIndex: {
            details[index] = {"result": input.details[index].result + " -> " + name + index,
                              "context": context};
            detailsChanged();
            cookAtIndexFinished(index, Operation.Finished);                              
        }
    }
}        
