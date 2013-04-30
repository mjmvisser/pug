import Pug 1.0

Node {
    id: self

    property list<Node> data

    count: 1
    
    inputs: [
        Input { name: "data" }
    ]

    CookOperation.onCook: {
        details[0].data = {}
        for (var dataIndex = 0; dataIndex < data.length; dataIndex++) {
            for (var i = 0; i < data[dataIndex].details.length; i++) {
                for (var k in details[0].data[dataIndex].details[i].data) {
                    details[0].data[k] = details[0].data[dataIndex].details[i].data[k];
                }
            }
        }
        detailsChanged(); 
        
        CookOperation.cookFinished(Operation.Finished);
    }
}