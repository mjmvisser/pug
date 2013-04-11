import Pug 1.0

ShotgunEntity {
    id: publishEventEntity
    name: "publishEventEntity"
    shotgunEntity: "PublishEvent"
    shotgunFields: [
        projectField,
        versionField,
        variationField,
        pathField,
        linkField,
        stepField,
        createdByField,
        updatedByField            
    ]

    order: [{field_name: "sg_version", direction: "desc"}]
    limit: 1
    count: 1

    property Node project
    property Node link
    property Node step

    inputs: [
        Input { name: "project" },
        Input { name: "link" },
        Input { name: "step" }
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
        id: versionField
        name: "versionField"
        shotgunField: "sg_version"
    }
    
    ShotgunField {
        id: variationField
        name: "variationField"
        shotgunField: "sg_variation"
    }
    
    ShotgunField {
        id: pathField
        name: "pathField"
        shotgunField: "sg_path"
        type: ShotgunField.Path
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
        id: createdByField
        name: "createdByField"
        shotgunField: "created_by"
    }

    ShotgunField {
        id: updatedByField
        name: "updatedByField"
        shotgunField: "updated_by"
    }
}

