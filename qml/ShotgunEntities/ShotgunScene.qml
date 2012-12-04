import Pug 1.0

ShotgunEntity {
    name: "Scene"
    
    property var project

    inputs: Input { name: "project" }

    ShotgunField {
        name: "code"
        field: "SEQUENCE"
    }
    
    ShotgunField {
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
        link: project
    }
}
