import Pug 1.0

ShotgunEntity {
    id: sceneEntity
    name: "sceneEntity"
    shotgunEntity: "Scene"
    shotgunFields: [
        codeField,
        projectField
    ]
    
    property Node project

    inputs: Input { name: "project" }

    output: true

    ShotgunField {
        id: codeField
        name: "code"
        shotgunField: "code"
        required: true
        field: "SEQUENCE"
        source: sceneEntity.parent
    }
    
    ShotgunField {
        id: projectField
        name: "project"
        shotgunField: "project"
        required: true
        type: ShotgunField.Link
        link: project
    }
}