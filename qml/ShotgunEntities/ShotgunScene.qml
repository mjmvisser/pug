import Pug 1.0

ShotgunEntity {
    shotgunEntity: "Scene"
    
    property ShotgunEntity project

    inputs: Input { name: "project" }

    ShotgunField {
        name: "code"
        shotgunField: "code"
        field: "SEQUENCE"
    }
    
    ShotgunField {
        name: "project"
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }
}
