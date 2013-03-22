import Pug 1.0
import MokkoDepts 1.0
import ShotgunEntities 1.0

Root {
    logLevel: Log.Info
    
    Shotgun {
        id: shotgun
        baseUrl: "https://mokko.shotgunstudio.com"
        apiKey: "ef0623879e54c9f4f2d80f9502a7adea09bfcf8f"
        scriptName: "test"
    }
    
    operations: [
        UpdateOperation {
            id: update
            name: "update"
        },
        ShotgunOperation {
            id: shotgunPull
            name: "shotgunPull"
            mode: ShotgunOperation.Pull
            shotgun: shotgun
            dependencies: update
        },
        ListOperation {
            id: ls
            name: "ls"
            dependencies: shotgunPull
        },
        CookOperation {
            id: cook
            name: "cook"
            dependencies: shotgunPull
        },
        ReleaseOperation {
            id: release
            name: "release"
            sudo: Sudo {
                user: "prod"
            }
            dependencies: cook
            triggers: shotgunPush
        },
        ShotgunOperation {
            id: shotgunPush
            name: "shotgunPush"
            mode: ShotgunOperation.Push
            shotgun: shotgun
        },
        TractorOperation {
            id: tractor
            name: "tractor"
            mode: TractorOperation.Submit
            target: update
        }
    ]

    fields: [
        Field { name: "PROJECT"; env: "MOKKO_PROJECT_ID"; regexp: "[0-9]{3}_[a-z]+" },
        Field { name: "SEQUENCE"; env: "MOKKO_SEQUENCE" },
        Field { name: "SHOT"; env: "MOKKO_SHOT" },
        Field { name: "ASSET"; env: "MOKKO_ASSET_NAME" },
        Field { name: "ASSET_TYPE"; env: "MOKKO_ASSET_TYPE" },
        Field { name: "DEPARTMENT"; env: "MOKKO_DEPARTMENT" },
        Field { name: "USER"; env: "USER" },
        Field { name: "VARIATION"; env: "MOKKO_VARIATION" },
        FrameSpecField { name: "FRAME" },
        Field { name: "VERSION"; type: Field.Integer; width: 3 },
        Field { name: "FORMAT" },
        Field { name: "EXT" }
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
    
    Transfer {
        id: transfer
        name: "transfer"
        root: project
    }
    
    Folder {
        id: scene
        name: "scene"
    	pattern: "shots/{SEQUENCE}/"
    	root: project

        ShotgunScene {
            id: sg_scene
            name: "sg_scene"
            project: sg_project
        }
    }
    
    Folder {
        id: shot
        name: "shot"
    	pattern: "{SHOT}/"
    	root: scene

        ShotgunShot {
            id: sg_shot
            name: "sg_shot"
            project: sg_project
            scene: sg_scene
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
    
    // Tracking {
        // id: tracking
        // name: "tracking"
        // root: shot
    // }
    
    // Folder {
        // id: asset
        // name: "asset"
        // pattern: "assets/{ASSET_TYPE}/{ASSET}/"
        // root: project
    // }
//     
    // Modeling {
        // id: model
        // name: "model"
        // root: asset
    // }
}