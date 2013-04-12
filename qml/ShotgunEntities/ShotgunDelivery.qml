import Pug 1.0


ShotgunEntity {
    id: deliveryEntity
    name: "deliveryEntity"
    shotgunEntity: "Delivery"
    shotgunFields: [
        titleField,
        projectField
    ]
    
    property Node project

    inputs: Input { name: "project" }
    
    count: parent.count

    ShotgunField {
        id: titleField
        name: "titleField"
        shotgunField: "title"
        field: "TRANSFER"
        source: deliveryEntity.parent
    }

    ShotgunField {
        id: projectField
        name: "projectField"
        shotgunField: "project"
        required: true
        type: ShotgunField.Link
        link: project
    }
}
