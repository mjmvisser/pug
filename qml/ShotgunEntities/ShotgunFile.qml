import Pug 1.0

ShotgunEntity {
    name: "Attachment"

    property alias project: projectField.link
    property alias release: releaseField.link
    property alias thumbnailPath: thumbnailField.value
    property alias filmstripPath: filmstripField.value
    property var user

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
    }

    ShotgunField {
        id: filmstripField
        name: "filmstrip_image"
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
