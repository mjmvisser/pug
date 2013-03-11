import Pug 1.0

ShotgunEntity {
    name: "PublishEvent"
    
    property var project
    property var entity
    property string code 
    property var user

    inputs: [
        Input { name: "project" },
        Input { name: "entity" },
        Input { name: "user" }
    ]
    
    params: [
        Param { name: "code" }
    ]

    ShotgunField {
        name: "project"
        type: ShotgunField.Link
        link: project
    }

    ShotgunField {
        name: "code"
        type: ShotgunField.Pattern
        pattern: code
    }

    ShotgunField {
        name: "sg_version"
        type: ShotgunField.Number
        field: "VERSION"
    }
    
    ShotgunField {
        name: "sg_variation"
        field: "VARIATION"
    }
    
    ShotgunField {
        name: "sg_path"
        type: ShotgunField.Path
    }

    ShotgunField {
        name: "sg_link"
        type: ShotgunField.Link
        link: entity
    }
    
    ShotgunField {
        name: "created_by"
        type: ShotgunField.Link
        link: user
    }

    ShotgunField {
        name: "updated_by"
        type: ShotgunField.Link
        link: user
    }
}

