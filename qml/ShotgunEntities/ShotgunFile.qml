import Pug 1.0

ShotgunEntity {
    id: fileEntity
    name: "fileEntity"
    shotgunEntity: "Attachment"
    shotgunFields: [
        thisFileField,
        projectField,
        linksField,
        imageField,
        filmstripField,
        createdByField,
        updatedByField                                    
    ]

    property Node project
    property Node release
    property Node thumbnail
    property Node filmstrip
    property Node user

    inputs: [
        Input { name: "project" },
        Input { name: "release" },
        Input { name: "thumbnail" },
        Input { name: "filmstrip" },
        Input { name: "user" }
    ]

    output: (action === ShotgunEntity.Create)

    count: parent.count

    params: [
        Param { name: "action" }
    ]

    ShotgunField {
        id: thisFileField
        name: "this_file"
        shotgunField: "this_file"
        urlType: ShotgunField.Local
        source: fileEntity.parent
    }

    ShotgunField {
        id: projectField
        name: "project"
        shotgunField: "project"
        required: true
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        id: linksField
        name: "attachment_links"
        shotgunField: "attachment_links"
        type: ShotgunField.MultiLink
        link: release
    }

    ShotgunField {
        id: imageField
        name: "image"
        shotgunField: "image"
        type: ShotgunField.Path
        source: thumbnail
    }

    ShotgunField {
        id: filmstripField
        name: "filmstrip_image"
        shotgunField: "filmstrip_image"
        type: ShotgunField.Path
        source: filmstrip
    }
    
    ShotgunField {
        id: createdByField
        name: "created_by"
        shotgunField: "created_by"
        type: ShotgunField.Link
        link: user
    }

    ShotgunField {
        id: updatedByField
        name: "updated_by"
        shotgunField: "updated_by"
        type: ShotgunField.Link
        link: user
    }
}
