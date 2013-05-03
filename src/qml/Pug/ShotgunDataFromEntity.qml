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
    
    ReleaseOperation.onCook: {
        count = entity.count;
        for (index = 0; index < count; index++) {
            details[index].data = {};
            details[index].data[shotgunField] = {"type": entity.details[index].entity.type,
                                                 "id": entity.details[index].entity.id};
        }
        detailsChanged();
        ReleaseOperation.cookFinished();
    }
}
