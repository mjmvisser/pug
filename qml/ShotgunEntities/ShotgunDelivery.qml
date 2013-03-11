import Pug 1.0

ShotgunEntity {
    name: "Delivery"

    property var project

    inputs: Input { name: "project" }

    ShotgunField {
        name: "title"
        field: "TRANSFER"
    }

    ShotgunField {
        name: "project"
        type: ShotgunField.Link
        link: project
    }
}
