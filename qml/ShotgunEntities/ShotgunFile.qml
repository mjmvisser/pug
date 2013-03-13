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
        shotgunField: "this_file"
        urlType: ShotgunField.Local
    }

    ShotgunField {
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        shotgunField: "attachment_links"
        type: ShotgunField.MultiLink
        link: release
    }

    ShotgunField {
        shotgunField: "image"
        type: ShotgunField.Path
        file: thumbnail
    }

    ShotgunField {
        shotgunField: "filmstrip_image"
        type: ShotgunField.Path
        file: thumbnail
    }
    
    ShotgunField {
        shotgunField: "created_by"
        type: ShotgunField.Link
        link: user
    }

    ShotgunField {
        shotgunField: "updated_by"
        type: ShotgunField.Link
        link: user
    }
}
