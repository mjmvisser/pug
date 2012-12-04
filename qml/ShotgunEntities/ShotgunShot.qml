import Pug 1.0

ShotgunEntity {
    name: "Shot"

    property var project
    property var scene: null
    property var sequence: null
    
    inputs: [
        Input { name: "project" },
        Input { name: "scene" },
        Input { name: "sequence" }
    ]
    
    ShotgunField {
        name: "code"
        field: "SHOT"
    }
    
    ShotgunField {
        name: "sg_scene"
        type: ShotgunField.Link
        linkType: "Scene"
        link: scene
    }
    
    ShotgunField {
        name: "sg_sequence"
        type: ShotgunField.Link
        linkType: "Sequence"
        link: sequence
    }
    
    ShotgunField {
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
        link: project
    }
    
    ShotgunField {
        name: "sg_head_in"
        type: ShotgunField.Number
    }
    
    ShotgunField {
        name: "sg_tail_out"
        type: ShotgunField.Number
    }
}
