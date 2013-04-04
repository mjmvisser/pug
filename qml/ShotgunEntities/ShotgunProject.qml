import Pug 1.0

ShotgunEntity {
    id: projectEntity
    name: "projectEntity"
    shotgunEntity: "Project"
    shotgunFields: [
        nameField
    ]    

    output: true

    ShotgunField {
        id: nameField
        name: "nameField"
        shotgunField: "name"
        required: true
        field: "PROJECT"
        source: projectEntity.parent
    }
}