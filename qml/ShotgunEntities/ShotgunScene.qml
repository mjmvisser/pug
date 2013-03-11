import Pug 1.0

ShotgunEntity {
    shotgunEntity: "Scene"
    
    property var project

    inputs: Input { name: "project" }

    ShotgunField {
        shotgunField: "code"
        field: "SEQUENCE"
    }
    
    ShotgunField {
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }
}
