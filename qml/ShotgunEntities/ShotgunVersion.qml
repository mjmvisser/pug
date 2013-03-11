import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

ShotgunEntity {
    shotgunEntity: "Version"

    property var project
    property string entityType
    property var entity
    property var release
    property string code
    property var thumbnail
    property var filmstrip 
    property var user

    inputs: [
        Input { name: "project" },
        Input { name: "entity" },
        Input { name: "release" },
        Input { name: "thumbnail" },
        Input { name: "filmstrip" },
        Input { name: "user" }
    ]

    params: [
        Param { name: "entityType" },
        Param { name: "code" }
    ]
    
    ShotgunField {
        shotgunField: "code"
        type: ShotgunField.Pattern
        pattern: code
    }

    ShotgunField {
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        id: entityField
        shotgunField: "entity"
        type: ShotgunField.Link
        link: entity
    }

    ShotgunField {
        shotgunField: "description"
    }
    
    ShotgunField {
        shotgunField: "sg_path_to_frames"
        type: ShotgunField.Path
    }

    ShotgunField {
        shotgunField: "sg_first_frame"
        type: ShotgunField.Number
        value: ShotgunUtils.safeElementAttribute(parent, "firstFrame")                         

    }

    ShotgunField {
        shotgunField: "sg_last_frame"
        type: ShotgunField.Number
        value: ShotgunUtils.safeElementAttribute(parent, "lastFrame")                         
    }
    
    ShotgunField {
        shotgunField: "sg_release"
        type: ShotgunField.Link
        link: release
    }

    ShotgunField {
        shotgunField: "image"
        value: ShotgunUtils.safeElementAttribute(thumbnail, "path")
    }

    ShotgunField {
        shotgunField: "filmstrip_image"
        value: ShotgunUtils.safeElementAttribute(filmstrip, "path")
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
