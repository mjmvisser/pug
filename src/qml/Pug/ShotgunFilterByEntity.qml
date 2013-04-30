import Pug 1.0

Node {
    id: self
    property Node entity
    property string shotgunField
    
    inputs: [
        Input { name: "entity" }
    ]
    
    params: [
        Param { name: "shotgunField" }
    ]
    
    UpdateOperation.onCook: {
        var entities = [];
        for (index = 0; index < entity.count; index++) {
            entities.push({"type": entity.details[index].entity.type,
                           "id": entity.details[index].entity.id});
        }
        
        if (entities.length > 0) {
            count = 1;
            if (entities.length == 1)
                details[0].filter = [shotgunField, "is", entities[0]];    
            else if (entities.length > 1)
                details[0].filter = [shotgunField, "in", entities];
            detailsChanged();
        } else {
            count = 0;
        }
        
        UpdateOperation.cookFinished(Operation.Finished);
    }
}
