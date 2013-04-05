import Pug 1.0

ShotgunEntity {
    id: sequenceEntity
    name: "sequenceEntity"
    shotgunEntity: "Sequence"
    shotgunFields: [
        codeField,
        projectField
    ]
    
    property Node project

    inputs: Input { name: "project" }

    output: (action === ShotgunEntity.Create)

    count: parent.count

    ShotgunField {
        id: codeField
        name: "code"
        shotgunField: "code"
        required: true
        field: "SEQUENCE"
        source: sequenceEntity.parent
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