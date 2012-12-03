import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

ShotgunEntity {
    name: "Attachment"

    property var project
    property var release
    property var thumbnail
    property var filmstrip
    property var user

    ShotgunUrlField {
        id: thisFileField
        name: "this_file"
        linkType: ShotgunUrlField.Local
    }

    ShotgunField {
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
        link: project
    }
    
    ShotgunField {
        name: "publish_event_sg_files_publish_events"
        type: ShotgunField.MultiLink
        linkType: "PublishEvent"
        link: release
    }

    ShotgunField {
        id: thumbnailField
        name: "image"
        value: ShotgunUtils.safeElementAttribute(thumbnail, "path")
    }

    ShotgunField {
        id: filmstripField
        name: "filmstrip_image"
        value: ShotgunUtils.safeElementAttribute(filmstrip, "path")
    }
    
    ShotgunField {
        name: "created_by"
        type: ShotgunField.Link
        link: user
        linkType: "HumanUser"
    }

    ShotgunField {
        name: "updated_by"
        type: ShotgunField.Link
        link: user
        linkType: "HumanUser"
    }
}
