import Pug 1.0

ShotgunEntity {
    id: self
    shotgunEntity: "PublishEvent"

    ShotgunOperation.action: ShotgunOperation.Create
    
    property ShotgunEntity project
    property ShotgunEntity link
    property ShotgunEntity step
    property Node thumbnail
    property Node filmstrip 
    property ShotgunEntity user

    inputs: [
        Input { name: "project" },
        Input { name: "link" },
        Input { name: "step" },
        Input { name: "thumbnail" },
        Input { name: "filmstrip" },
        Input { name: "user" }
    ]

    property string code 
    
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
        link: self.link
    }

    ShotgunField {
        id: stepField
        name: "sg_step"
        shotgunField: "sg_step"
        type: ShotgunField.Link
        link: step
    }
    
    ShotgunField {
        name: "sg_image"
        shotgunField: "image"
        value: (thumbnail && thumbnail.details[index]) ? thumbnail.details[index].element.pattern : 0
    }

    ShotgunField {
        name: "sg_filmstrip"
        shotgunField: "filmstrip_image"
        value: (filmstrip && filmstrip.details[index]) ? filmstrip.details[index].element.pattern : 0
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

