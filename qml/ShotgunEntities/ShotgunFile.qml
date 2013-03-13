import Pug 1.0

ShotgunEntity {
    shotgunEntity: "Attachment"

    property var project
    property var release
    property Node thumbnail
    property Node filmstrip
    property var user

    inputs: [
        Input { name: "project" },
        Input { name: "release" },
        Input { name: "thumbnail" },
        Input { name: "filmstrip" },
        Input { name: "user" }
    ]

    ShotgunField {
        name: "this_file"
        shotgunField: "this_file"
        urlType: ShotgunField.Local
    }

    ShotgunField {
        name: "project"
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        name: "attachment_links"
        shotgunField: "attachment_links"
        type: ShotgunField.MultiLink
        link: release
    }

    ShotgunField {
        name: "image"
        shotgunField: "image"
        type: ShotgunField.Path
        file: thumbnail
    }

    ShotgunField {
        name: "filmstrip_image"
        shotgunField: "filmstrip_image"
        type: ShotgunField.Path
        file: thumbnail
    }
    
    ShotgunField {
        name: "created_by"
        shotgunField: "created_by"
        type: ShotgunField.Link
        link: user
    }

    ShotgunField {
        name: "updated_by"
        shotgunField: "updated_by"
        type: ShotgunField.Link
        link: user
    }
}
