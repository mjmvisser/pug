import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

ShotgunEntity {
    shotgunEntity: "Version"

    property var project
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
        Param { name: "code" }
    ]
    
    ShotgunField {
        name: "code"
        shotgunField: "code"
        type: ShotgunField.Pattern
        pattern: code
    }

    ShotgunField {
        name: "project"
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        id: entityField
        name: "entity"
        shotgunField: "entity"
        type: ShotgunField.Link
        link: entity
    }

    ShotgunField {
        name: "description"
        shotgunField: "description"
    }
    
    ShotgunField {
        name: "sg_path_to_frames"
        shotgunField: "sg_path_to_frames"
        type: ShotgunField.Path
    }

    ShotgunField {
        name: "sg_first_frame"
        shotgunField: "sg_first_frame"
        type: ShotgunField.Number
        value: parent.details[index].element.firstFrame                         

    }

    ShotgunField {
        name: "sg_last_frame"
        shotgunField: "sg_last_frame"
        type: ShotgunField.Number
        value: parent.details[index].element.firstFrame                         
    }
    
    ShotgunField {
        name: "sg_release"
        shotgunField: "sg_release"
        type: ShotgunField.Link
        link: release
    }

    ShotgunField {
        name: "image"
        shotgunField: "image"
        value: ShotgunUtils.safeElementAttribute(thumbnail, "path")
    }

    ShotgunField {
        name: "filmstrip_image"
        shotgunField: "filmstrip_image"
        value: ShotgunUtils.safeElementAttribute(filmstrip, "path")
    }

    ShotgunField {
        name: "created_by"
        shotgunField: "created_by"
        type: ShotgunField.Link
        link: user
    }

    ShotgunField {
        name: "updated_by"
        shotgunField: "updated_by"
        type: ShotgunField.Link
        link: user
    }
}
