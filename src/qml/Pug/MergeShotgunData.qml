import Pug 1.0

Node {
    id: self

    property list<Node> data

    inputs: [
        Input { name: "data" }
    ]

    ReleaseOperation.onCook: {
        count = 1;
        details[0].data = {};
        for (var dataIndex = 0; dataIndex < data.length; dataIndex++) {
            for (var i = 0; i < data[dataIndex].details.length; i++) {
                for (var k in data[dataIndex].details[i].data) {
                    details[0].data[k] = data[dataIndex].details[i].data[k];
                }
            }
        }
        detailsChanged(); 
        ReleaseOperation.cookFinished();
    }
}