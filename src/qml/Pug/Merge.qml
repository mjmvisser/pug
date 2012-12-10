import Pug 1.0

Node {
    id: self
    property var input1: null
    property var input2: null
    property var input3: null
    
    inputs: [
        Input { name: "input1" },
        Input { name: "input2" },
        Input { name: "input3" }
    ]
    
    signal cook(var env);
    signal cooked(int status);
    
    onCook: {
        // merge inputs
        if (input1) {
            for (var index = 0; index < input1.details.length; index++) {
                details.push(input1.details[index]);
            }
        }
        
        if (input2) {
            for (var index = 0; index < input2.details.length; index++) {
                details.push(input2.details[index]);
            }
        }
        
        if (input3) {
            for (var index = 0; index < input3.details.length; index++) {
                details.push(input3.details[index]);
            }
        }
        
        detailsChanged();
        cooked(Operation.Finished);
    } 
}
