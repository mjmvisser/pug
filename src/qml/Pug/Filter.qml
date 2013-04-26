import Pug 1.0

Node {
    id: self
    property Node source
    property bool select: true
    
    inputs: Input { name: "source" }

    function filter() {
        for (index = 0; index < source.details.length; index++) {
            if (select) {
                details.push(source.details[index]);
            }
        }
        count = details.length;
        detailsChanged();
    }
    
    UpdateOperation.onCook: {
        filter();
        UpdateOperation.cookFinished(Operation.Finished);
    }
    
    CookOperation.onCook: {
        filter();
        CookOperation.cookFinished(Operation.Finished);
    }
}
