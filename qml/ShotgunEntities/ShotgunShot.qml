import Pug 1.0

ShotgunEntity {
    shotgunEntity: "Shot"

    property var project
    property var scene
    property var sequence
    
    
    inputs: [
        Input { name: "project" },
        Input { name: "scene" },
        Input { name: "sequence" }
    ]
    
    ShotgunField {
        name: "code"
        shotgunField: "code"
        field: "SHOT"
    }
    
    ShotgunField {
        name: "sg_scene"
        shotgunField: "sg_scene"
        type: ShotgunField.Link
        link: scene
    }
    
    ShotgunField {
        name: "sg_sequence"
        shotgunField: "sg_sequence"
        type: ShotgunField.Link
        link: sequence
    }
    
    ShotgunField {
        name: "project"
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        name: "sg_head_in"
        shotgunField: "sg_head_in"
        type: ShotgunField.Number
    }
    
    ShotgunField {
        name: "sg_tail_out"
        shotgunField: "sg_tail_out"
        type: ShotgunField.Number
    }
}
