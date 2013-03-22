import Pug 1.0

ShotgunEntity {
    shotgunEntity: "Delivery"

    property ShotgunEntity project

    inputs: Input { name: "project" }

    ShotgunField {
        name: "title"
        shotgunField: "title"
        field: "TRANSFER"
    }

    ShotgunField {
        name: "project"
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }
}
