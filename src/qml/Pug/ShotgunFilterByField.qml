import Pug 1.0

Node {
    id: self
    property Branch branch
    property string shotgunField
    property string field
    
    inputs: [
        Input { name: "branch" }
    ]
    
    params: [
        Param { name: "shotgunField" },
        Param { name: "field" }
    ]
    
    UpdateOperation.onCook: {
        try {
            var values = [];
            for (index = 0; index < branch.count; index++) {
                values.push(branch.formatFields("{" + field + "}", 
                                                Util.mergeContexts(context, branch.details[index].context)));
            }
            
            if (values.length > 0) {
                count = 1;
                if (values.length == 1)
                    details[0].filter = [shotgunField, "is", values[0]];    
                else if (values.length > 1)
                    details[0].filter = [shotgunField, "in", values];
                detailsChanged();
            } else {
                count = 0;
            }
            
            UpdateOperation.cookFinished(Operation.Finished);
        } catch (e) {
            addError("Cook failed: " + e);
            UpdateOperation.cookFinished(Operation.Error);
        }
    }
}
