import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

ShotgunEntity {
    shotgunEntity: "Attachment"

    property var project
    property var release
    property var thumbnail
    property var filmstrip
    property var user

    inputs: [
        Input { name: "project" },
        Input { name: "release" },
        Input { name: "thumbnail" },
        Input { name: "filmstrip" },
        Input { name: "user" }
    ]

    ShotgunField {
        id: thisFileField
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
        id: thumbnailField
        shotgunField: "image"
        value: thumbnail
    }

    ShotgunField {
        id: filmstripField
        shotgunField: "filmstrip_image"
        value: thumbnail
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
