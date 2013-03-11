import Pug 1.0

ShotgunEntity {
    shotgunEntity: "Delivery"

    property var project

    inputs: Input { name: "project" }

    ShotgunField {
        shotgunField: "title"
        field: "TRANSFER"
    }

    ShotgunField {
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }
}
