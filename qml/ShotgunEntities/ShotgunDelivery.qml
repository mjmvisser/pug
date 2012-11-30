import Pug 1.0

ShotgunEntity {
    name: "Delivery"

    property alias project: project.link

    ShotgunField {
        name: "title"
        field: "TRANSFER"
    }

    ShotgunField {
        id: project
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
    }
}
