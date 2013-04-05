import Pug 1.0

ShotgunEntity {
    id: publishEventEntity
    name: "publishEventEntity"
    shotgunEntity: "PublishEvent"
    shotgunFields: [
        projectField,
        codeField,
        versionField,
        variationField,
        pathField,
        linkField,
        stepField,
        imageField,
        filmstripField,
        dependenciesField,
        createdByField,
        updatedByField            
    ]

    property Node project
    property Node link
    property Node step
    property Node thumbnail: null
    property Node filmstrip: null
    property Node user
    property list<Node> dependencies

    inputs: [
        Input { name: "project" },
        Input { name: "link" },
        Input { name: "step" },
        Input { name: "thumbnail" },
        Input { name: "filmstrip" },
        Input { name: "user" },
        Input { name: "dependencies" }
    ]

    output: (action === ShotgunEntity.Create)

    count: parent.count

    property string code
    property string category
    property string resolution

    params: [
        Param { name: "code" },
        Param { name: "category" },
        Param { name: "resolution" }
    ]

    ShotgunField {
        id: projectField
        name: "projectField"
        shotgunField: "project"
        required: true
        type: ShotgunField.Link
        link: project
    }

    ShotgunField {
        id: codeField
        name: "codeField"
        shotgunField: "code"
        required: true
        type: ShotgunField.String
        pattern: code
        source: publishEventEntity.parent
    }

    ShotgunField {
        id: categoryField
        name: "categoryField"
        type: ShotgunField.String
        value: category
    }

    ShotgunField {
        id: resolutionField
        name: "resolutionField"
        type: ShotgunField.String
        value: resolution
    }

    ShotgunField {
        id: versionField
        name: "versionField"
        shotgunField: "sg_version"
        type: ShotgunField.Number
        field: "VERSION"
        source: publishEventEntity.parent
    }
    
    ShotgunField {
        id: variationField
        name: "variationField"
        shotgunField: "sg_variation"
        field: "VARIATION"
        source: publishEventEntity.parent
    }
    
    ShotgunField {
        id: pathField
        name: "pathField"
        shotgunField: "sg_path"
        type: ShotgunField.Path
        source: publishEventEntity.parent
    }

    ShotgunField {
        id: linkField
        name: "linkField"
        shotgunField: "sg_link"
        type: ShotgunField.Link
        link: publishEventEntity.link
    }

    ShotgunField {
        id: stepField
        name: "stepField"
        shotgunField: "sg_step"
        type: ShotgunField.Link
        link: step
    }
    
    ShotgunField {
        id: imageField
        name: "imageField"
        shotgunField: "image"
        type: ShotgunField.Path
        source: thumbnail
    }

    ShotgunField {
        id: filmstripField
        name: "filmstripField"
        shotgunField: "filmstrip_image"
        type: ShotgunField.Path
        source: filmstrip
    }

    ShotgunField {
        id: dependenciesField
        name: "dependenciesField"
        shotgunField: "sg_dependencies"
        type: ShotgunField.MultiLink
        links: dependencies
    }

    ShotgunField {
        id: createdByField
        name: "createdByField"
        shotgunField: "created_by"
        type: ShotgunField.Link
        link: user
    }

    ShotgunField {
        id: updatedByField
        name: "updatedByField"
        shotgunField: "updated_by"
        type: ShotgunField.Link
        link: user
    }
}

