import Pug 1.0

ShotgunEntity {
    shotgunEntity: "PublishEvent"
    
    property ShotgunEntity project
    property ShotgunEntity entity
    property string code 
    property var thumbnail
    property var filmstrip 
    property ShotgunEntity user

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
        name: "project"
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }

    ShotgunField {
        name: "code"
        shotgunField: "code"
        type: ShotgunField.Pattern
        pattern: code
    }

    ShotgunField {
        name: "sg_version"
        shotgunField: "sg_version"
        type: ShotgunField.Number
        field: "VERSION"
    }
    
    ShotgunField {
        name: "sg_variation"
        shotgunField: "sg_variation"
        field: "VARIATION"
    }
    
    ShotgunField {
        name: "sg_path"
        shotgunField: "sg_path"
        type: ShotgunField.Path
    }

    ShotgunField {
        name: "sg_link"
        shotgunField: "sg_link"
        type: ShotgunField.Link
        link: entity
    }
    
    ShotgunField {
        name: "sg_image"
        shotgunField: "image"
        value: thumbnail ? thumbnail.details[index].element.path : 0
    }

    ShotgunField {
        name: "sg_filmstrip"
        shotgunField: "filmstrip_image"
        value: filmstrip ? filmstrip.details[index].element.path : 0
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

