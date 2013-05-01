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
    
    CookOperation.onCook: {
        for (index = 0; index < entity.count; index++) {
            details[index].data = {};
            details[index].data[shotgunField] = {"type": entity.details[index].entity.type,
                                                 "id": entity.details[index].entity.id};
        }
        detailsChanged();
        CookOperation.cookFinished();
    }
}
