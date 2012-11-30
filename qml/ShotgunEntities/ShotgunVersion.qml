import QtQuick 2.0
import Pug 1.0

ShotgunEntity {
    name: "Version"

    property alias project: projectField.link
    property alias entity: entityField
    property alias release: releaseField.link
    property alias code: codeField.pattern
    property alias firstFrame: firstFrameField.value
    property alias lastFrame: lastFrameField.value
    property alias thumbnailPath: thumbnailField.value
    property alias filmstripPath: filmstripField.value
    property var user
    
    ShotgunField {
        id: codeField
        name: "code"
        type: ShotgunField.Pattern
    }

    ShotgunField {
        id: projectField
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
    }
    
    
    ShotgunField {
        id: entityField
        name: "entity"
        type: ShotgunField.Link
    }

    ShotgunField {
        name: "description"
    }
    
    ShotgunField {
        name: "sg_path_to_frames"
        type: ShotgunField.Path
    }

    ShotgunField {
        id: firstFrameField
        name: "sg_first_frame"
        type: ShotgunField.Number
    }

    ShotgunField {
        id: lastFrameField
        name: "sg_last_frame"
        type: ShotgunField.Number
    }
    
    ShotgunField {
        id: releaseField
        name: "sg_release"
        type: ShotgunField.Link
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
