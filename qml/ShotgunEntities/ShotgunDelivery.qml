import Pug 1.0

ShotgunEntity {
    name: "Delivery"

    property var project

    ShotgunField {
        name: "title"
        field: "TRANSFER"
    }

    ShotgunField {
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
        link: project
    }
}
