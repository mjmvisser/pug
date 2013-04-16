import QtQuick 2.0
import Pug 1.0
import MokkoDepts 1.0
import ShotgunEntities 1.0

Root {
    logLevel: Log.Info
    
    
    Component.onCompleted: {
        Shotgun.baseUrl = "https://mokko.shotgunstudio.com";
        Shotgun.apiKey = "eefeed79c47b4630bd8acf326bec745dc9bfb73a";
        Shotgun.scriptName = "pug";
    }
    
    operations: [
        UpdateOperation {
            id: update
            name: "update"
        },
        ListOperation {
            id: ls
            name: "ls"
            dependencies: update
        },
        CookOperation {
            id: cook
            name: "cook"
            dependencies: update
        },
        ReleaseOperation {
            id: release
            name: "release"
            sudo: Sudo {
                user: "prod"
            }
            dependencies: cook
        },
        TractorOperation {
            id: tractor
            name: "tractor"
            mode: TractorOperation.Submit
            target: release
        }
    ]

    fields: [
        Field { name: "PROJECT"; env: "MOKKO_PROJECT_ID"; regexp: "[0-9]{3}_[a-z0-9]+" },
        Field { name: "SEQUENCE"; env: "MOKKO_SEQUENCE" },
        Field { name: "SHOT"; env: "MOKKO_SHOT" },
        Field { name: "ASSET"; env: "MOKKO_ASSET_NAME" },
        Field { name: "ASSET_TYPE"; env: "MOKKO_ASSET_TYPE" },
        Field { name: "DEPARTMENT"; env: "MOKKO_DEPARTMENT" },
        Field { name: "USER"; env: "USER" },
        Field { name: "VARIATION"; env: "MOKKO_VARIATION" },
        FrameSpecField { name: "FRAME" },
        Field { name: "VERSION"; type: Field.Integer; width: 3; regexp: "[0-9]{3}" },
        Field { name: "FORMAT" },
        Field { name: "EXT" },
        Field { name: "RESOLUTION"; regexp: "[0-9]+x[0-9]+" }
    ]
    
    ShotgunHumanUser {
        id: sg_user
        name: "sg_user"
    }

    Folder {
        id: prod
    	name: "root"
    	pattern: "/prod/projects/"
    }

    Folder {
        id: project
    	name: "project"
    	pattern: "{PROJECT}/"
        root: prod

        ShotgunProject {
            id: sg_project
            name: "sg_project"
        }
    }
    
    // Transfer {
        // id: transfer
        // name: "transfer"
        // root: project
    // }
    
    Folder {
        id: sequence
        name: "sequence"
    	pattern: "shots/{SEQUENCE}/"
    	root: project

        ShotgunSequence {
            id: sg_sequence
            name: "sg_sequence"
            project: sg_project
        }
    }
    
    Folder {
        id: shot
        name: "shot"
    	pattern: "{SHOT}/"
    	root: sequence

        ShotgunShot {
            id: sg_shot
            name: "sg_shot"
            project: sg_project
            sequence: sg_sequence
        }
    }
    
    // Footage {
        // id: footage
        // name: "footage"
        // root: shot
    // }
    
    Plates {
        id: plate
        name: "plate"
        root: shot
    }
    
    Tracking {
        id: tracking
        name: "tracking"
        root: shot
    }
    
    // Folder {
        // id: asset
        // name: "asset"
        // pattern: "assets/{ASSET_TYPE}/{ASSET}/"
        // root: project
    // }
    
    // Modeling {
        // id: model
        // name: "model"
        // root: asset
    // }
}