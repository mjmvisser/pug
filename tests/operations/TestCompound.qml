import Pug 1.0

Node {
    id: self
    property NodeBase input

    inputs: Input { name: "input" }

    count: input.count

    details: nodeC.details

    Node {
        id: nodeB
        name: "nodeB"

        property NodeBase input: self.input
        
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

    Node {
        id: nodeC
        name: "nodeC"
        property NodeBase input: nodeB
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
