import Pug 1.0

ShotgunEntity {
    shotgunEntity: "PublishEvent"
    
    property var project
    property var entity
    property string code 
    property var thumbnail
    property var filmstrip 
    property var user

    inputs: [
        Input { name: "project" },
        Input { name: "entity" },
        Input { name: "thumbnail" },
        Input { name: "filmstrip" },
        Input { name: "user" }
    ]
    
    params: [
        Param { name: "code" }
    ]

    ShotgunField {
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }

    ShotgunField {
        shotgunField: "code"
        type: ShotgunField.Pattern
        pattern: code
    }

    ShotgunField {
        shotgunField: "sg_version"
        type: ShotgunField.Number
        field: "VERSION"
    }
    
    ShotgunField {
        shotgunField: "sg_variation"
        field: "VARIATION"
    }
    
    ShotgunField {
        shotgunField: "sg_path"
        type: ShotgunField.Path
    }

    ShotgunField {
        shotgunField: "sg_link"
        type: ShotgunField.Link
        link: entity
    }
    
    ShotgunField {
        shotgunField: "image"
        value: thumbnail ? thumbnail.details[index].element.path : 0
    }

    ShotgunField {
        shotgunField: "filmstrip_image"
        value: filmstrip ? filmstrip.details[index].element.path : 0
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

