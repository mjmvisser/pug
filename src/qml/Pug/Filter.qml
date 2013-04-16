import Pug 1.0

Node {
    id: self
    property Node source
    property bool select: true
    
    inputs: Input { name: "source" }

    signal update(var context)
    signal updateFinished(int status)

    signal cook(var context)
    signal cookFinished(int status)
    
    function filter() {
        for (index = 0; index < source.details.length; index++) {
            if (select) {
                details.push(source.details[index]);
            }
        }
        count = details.length;
        detailsChanged();
    }
    
    onUpdate: {
        filter();
        updateFinished(Operation.Finished);
    }
    
    onCook: {
        filter();
        cookFinished(Operation.Finished);
    }
}
