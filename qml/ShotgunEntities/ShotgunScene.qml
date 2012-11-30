import Pug 1.0

ShotgunEntity {
    name: "Scene"
    
    property alias project: projectField.link

    ShotgunField {
        name: "code"
        field: "SEQUENCE"
    }
    
    ShotgunField {
        id: projectField
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
    }
}
