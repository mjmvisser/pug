import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

ShotgunEntity {
    name: "Version"

    property var project
    property string entityType
    property var entity
    property var release
    property string code
    property var thumbnail
    property var filmstrip 
    property var user
    
    ShotgunField {
        name: "code"
        type: ShotgunField.Pattern
        pattern: code
    }

    ShotgunField {
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
        link: project
    }
    
    ShotgunField {
        id: entityField
        name: "entity"
        type: ShotgunField.Link
        linkType: entityType
        link: entity
    }

    ShotgunField {
        name: "description"
    }
    
    ShotgunField {
        name: "sg_path_to_frames"
        type: ShotgunField.Path
    }

    ShotgunField {
        name: "sg_first_frame"
        type: ShotgunField.Number
        value: ShotgunUtils.safeElementAttribute(parent, "firstFrame")                         

    }

    ShotgunField {
        name: "sg_last_frame"
        type: ShotgunField.Number
        value: ShotgunUtils.safeElementAttribute(parent, "lastFrame")                         
    }
    
    ShotgunField {
        name: "sg_release"
        type: ShotgunField.Link
        linkType: "PublishEvent"
        link: release
    }

    ShotgunField {
        name: "image"
        value: ShotgunUtils.safeElementAttribute(thumbnail, "path")
    }

    ShotgunField {
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
