import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

ShotgunEntity {
    name: "Version"

    property alias project: projectField.link
    property alias entity: entityField
    property alias release: releaseField.link
    property alias code: codeField.pattern
    property var thumbnail: null
    property var filmstrip: null 
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
        value: ShotgunUtils.safeElementAttribute(parent, "firstFrame")                         

    }

    ShotgunField {
        id: lastFrameField
        name: "sg_last_frame"
        type: ShotgunField.Number
        value: ShotgunUtils.safeElementAttribute(parent, "lastFrame")                         
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
