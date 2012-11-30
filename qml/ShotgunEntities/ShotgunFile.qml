import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

ShotgunEntity {
    name: "Attachment"

    property alias project: projectField.link
    property alias release: releaseField.link
    property var thumbnail: null
    property var filmstrip: null
    property var user: null

    ShotgunUrlField {
        id: thisFileField
        name: "this_file"
        linkType: ShotgunUrlField.Local
    }

    ShotgunField {
        id: projectField
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
    }
    
    ShotgunField {
        id: releaseField
        name: "publish_event_sg_files_publish_events"
        type: ShotgunField.MultiLink
        linkType: "PublishEvent"
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
