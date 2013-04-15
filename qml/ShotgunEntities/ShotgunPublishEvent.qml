import Pug 1.0
import MokkoTools 1.0

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
        updatedByField,
        categoryField,
        resolutionField          
    ]

    property Node project
    property Node link
    property Node step
    property Node user
    property list<Node> dependencies
    property Node frames

    inputs: [
        Input { name: "project" },
        Input { name: "link" },
        Input { name: "step" },
        Input { name: "user" },
        Input { name: "dependencies" },
        Input { name: "frames" }
    ]

    output: true

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
        shotgunField: "sg_category"
        type: ShotgunField.String
        value: category
    }

    ShotgunField {
        id: resolutionField
        name: "resolutionField"
        shotgunField: "sg_res"
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
    
    ShotgunThumbnail {
        id: thumbnail
        name: "thumbnail"
        input: frames
    }

    ShotgunField {
        id: imageField
        name: "imageField"
        shotgunField: "image"
        type: ShotgunField.Path
        source: frames ? thumbnail : null
    }

    ShotgunThumbnail {
        id: filmstrip
        name: "filmstrip"
        input: frames
        filmstrip: true
    }

    ShotgunField {
        id: filmstripField
        name: "filmstripField"
        shotgunField: "filmstrip_image"
        type: ShotgunField.Path
        source: frames ? filmstrip : null
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

