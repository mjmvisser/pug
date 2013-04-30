import Pug 1.0

ShotgunFind {
    id: self
    entityType: "HumanUser"
    fields: ["login"]
    limit: 1

    inputs: [
        Input { name: "root" }
    ]
    
    property Root root
    
    UpdateOperation.onPrepare: {
        filters = [["login", "is", root.formatFields("{USER}", context)]];
        UpdateOperation.prepareFinished(Operation.Finished);
    }
}