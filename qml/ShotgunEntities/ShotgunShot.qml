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
        shotgunField: "code"
        field: "SHOT"
    }
    
    ShotgunField {
        shotgunField: "sg_scene"
        type: ShotgunField.Link
        link: scene
    }
    
    ShotgunField {
        shotgunField: "sg_sequence"
        type: ShotgunField.Link
        link: sequence
    }
    
    ShotgunField {
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        shotgunField: "sg_head_in"
        type: ShotgunField.Number
    }
    
    ShotgunField {
        shotgunField: "sg_tail_out"
        type: ShotgunField.Number
    }
}
