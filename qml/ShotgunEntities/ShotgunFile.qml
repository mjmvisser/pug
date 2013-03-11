import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

ShotgunEntity {
    name: "Attachment"

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
        name: "this_file"
        urlType: ShotgunField.Local
    }

    ShotgunField {
        name: "project"
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        name: "attachment_links"
        type: ShotgunField.MultiLink
        link: release
    }

    ShotgunField {
        id: thumbnailField
        name: "image"
        value: thumbnail
    }

    ShotgunField {
        id: filmstripField
        name: "filmstrip_image"
        value: thumbnail
    }
    
    ShotgunField {
        name: "created_by"
        type: ShotgunField.Link
        link: user
    }

    ShotgunField {
        name: "updated_by"
        type: ShotgunField.Link
        link: user
    }
}
