import QtQuick 2.0
import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Folder {
    id: delivery
    pattern: "delivery/to_client/{DELIVERY}/"

    fields: [
        Field { name: "DELIVERY"; regexp: "20\\d{6}" },
        Field { name: "VERSION_ID"; type: Field.Integer }
    ]
    
    ShotgunDelivery {
        id: sg_delivery
        project: node("/project/sg_project")
    }

    File {
        logLevel: Log.Trace
        id: nukeTemplate
        name: "nukeTemplate"
        pattern: "/home/mvisser/workspace/pug/qml/MokkoTools/scripts/cosmos_delivery_template.nk"
    }

    ShotgunEntity {
        id: sg_version
        name: "sg_version"
        shotgunEntity: "Version"
        count: 1
        shotgunFields: [
            ShotgunField {
                name: "id"
                shotgunField: "id"
                type: ShotgunField.Number
                field: "VERSION_ID"
            },
            ShotgunField {
                name: "project"
                shotgunField: "project"
                type: ShotgunField.Link
                link: node("/project/sg_project")
            },
            ShotgunField {
                name: "shotCode"
                shotgunField: "entity.Shot.code"
            },
            ShotgunField {
                name: "sequence"
                shotgunField: "entity.Shot.sg_sequence"
            },
            ShotgunField {
                name: "scene"
                shotgunField: "entity.Shot.sg_scene"
            },
            ShotgunField {
                name: "artist"
                shotgunField: "user.HumanUser.login"
            },
            ShotgunField {
                name: "description"
                shotgunField: "description"
            },
            ShotgunField {
                name: "pathToFrames"
                shotgunField: "sg_path_to_frames"
                type: ShotgunField.Path
            },
            ShotgunField {
                name: "clientVersion"
                shotgunField: "sg_version_client"
                type: ShotgunField.Number
            },
            ShotgunField {
                name: "step"
                shotgunField: "sg_step.Step.code"
            }
        ]
    }
    
    DeliveryQuicktime {
        id: generateQuicktime
        name: "generateQuicktime"
        nukeTemplate: nukeTemplate
        input: sg_version
        format: "HD"
        filetype: "mov"
        project: "Cosmos"
        sequence: input.details[index].entity["entity.Shot.sg_sequence"].name
        scene: (input.details[index].entity["entity.Shot.sg_scene"] || {}).name || ""
        version: input.details[index].entity["sg_version_client"]
        notes: input.details[index].entity["description"] || input.details[index].entity["sg_step.Step.code"]
        artist: input.details[index].entity["user.HumanUser.login"]
    }
    
    File {
        id: quicktime
        name: "quicktime"
        pattern: "{FILENAME}.mov"
        input: generateQuicktime
    }
}
