import Pug 1.0

ShotgunEntity {
    id: shotEntity
    name: "shotEntity"
    shotgunEntity: "Shot"
    TractorOperation.flatten: true
    shotgunFields: [
        codeField,
        sceneField,
        sequenceField,
        projectField,
        headInField,
        tailOutField
    ]
    
    property Node project
    property Node scene: null
    property Node sequence: null
    
    inputs: [
        Input { name: "project" },
        Input { name: "scene" },
        Input { name: "sequence" }
    ]
    
    count: parent.count

    ShotgunField {
        id: codeField
        name: "code"
        shotgunField: "code"
        required: true
        field: "SHOT"
        source: shotEntity.parent
    }
    
    ShotgunField {
        id: sceneField
        name: "sg_scene"
        shotgunField: "sg_scene"
        type: ShotgunField.Link
        link: scene
    }
    
    ShotgunField {
        id: sequenceField
        name: "sg_sequence"
        shotgunField: "sg_sequence"
        type: ShotgunField.Link
        link: sequence
    }
    
    ShotgunField {
        id: projectField
        name: "project"
        shotgunField: "project"
        required: true
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        id: headInField
        name: "sg_head_in"
        shotgunField: "sg_head_in"
        type: ShotgunField.Number
        source: shotEntity.parent
    }
    
    ShotgunField {
        id: tailOutField
        name: "sg_tail_out"
        shotgunField: "sg_tail_out"
        type: ShotgunField.Number
        source: shotEntity.parent
    }
}
