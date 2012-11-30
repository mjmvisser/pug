import Pug 1.0

ShotgunEntity {
    name: "Shot"

    property alias project: projectField.link
    property alias scene: sceneField.link
    property alias sequence: sequenceField.link
    
    ShotgunField {
        name: "code"
        field: "SHOT"
    }
    
    ShotgunField {
        id: sceneField
        name: "sg_scene"
        type: ShotgunField.Link
        linkType: "Scene"
    }
    
    ShotgunField {
        id: sequenceField
        name: "sg_sequence"
        type: ShotgunField.Link
        linkType: "Sequence"
    }
    
    ShotgunField {
        id: projectField
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
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
