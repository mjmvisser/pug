import Pug 1.0

ShotgunEntity {
    name: "PublishEvent"
    
    property var project
    property string entityType
    property var entity
    property string code 
    property var user

    inputs: [
        Input { name: "project" },
        Input { name: "entity" },
        Input { name: "user" }
    ]
    
    params: [
        Param { name: "entityType" },
        Param { name: "code" }
    ]

    ShotgunField {
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
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
        linkType: entityType
        link: entity
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

