import Pug 1.0

DeprecatedNode {
    id: self
    property Node input

    inputs: Input { name: "input" }

    count: input.count

    details: nodeC.details

    DeprecatedNode {
        id: nodeB
        name: "nodeB"

        property Node input: self.input
        
        inputs: Input { name: "input" }
        
        count: self.count
        
        signal cookAtIndex(int index, var context)
        signal cookedAtIndex(int index, int status)

        onCookAtIndex: {
            details[index] = {"result": input.details[index].result + " -> " + name + index,
                              "context": context};
            detailsChanged();
            cookedAtIndex(index, Operation.Finished);                              
        }
    }

    DeprecatedNode {
        id: nodeC
        name: "nodeC"
        property Node input: nodeB
        active: true
        
        inputs: Input { name: "input" }

        count: input.count
        
        signal cookAtIndex(int index, var context)
        signal cookedAtIndex(int index, int status)

        onCookAtIndex: {
            details[index] = {"result": input.details[index].result + " -> " + name + index,
                              "context": context};
            detailsChanged();
            cookedAtIndex(index, Operation.Finished);                              
        }
    }
}        
