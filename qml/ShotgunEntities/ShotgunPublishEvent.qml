import Pug 1.0

ShotgunEntity {
    name: "PublishEvent"
    
    property alias project: projectField.link
    property alias entity: entityField
    property alias code: codeField.pattern 
    property var user

    ShotgunField {
        id: projectField
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
    }

    ShotgunField {
        id: codeField
        name: "code"
        type: ShotgunField.Pattern
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
        id: entityField
        name: "sg_link"
        type: ShotgunField.Link
        linkType: "Delivery"
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

