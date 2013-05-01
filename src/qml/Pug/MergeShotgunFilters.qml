import Pug 1.0

Node {
    id: self

    property list<Node> filters
    property string filterOperator: "all"

    count: 1
    
    inputs: [
        Input { name: "filters" }
    ]

    params: [
        Param { name: "filterOperator" }
    ]

    UpdateOperation.onCook: {
        var filterList = [];
        for (var filterIndex = 0; filterIndex < filters.length; filterIndex++) {
            for (var i = 0; i < filters[filterIndex].details.length; i++) {
                filterList.push(filters[filterIndex].details[i].filter);
            }                
        }
        
        details[0].filter = {"filter_operator": filterOperator,
                             "filters": filterList};
        detailsChanged(); 
        
        UpdateOperation.cookFinished();
    }
}