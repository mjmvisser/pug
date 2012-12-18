import Pug 1.0
import MokkoDepts 1.0
import ShotgunEntities 1.0

Root {
    //logLevel: Log.Copious
    
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
            dependencies: cook
            triggers: shotgunPush
        },
        ShotgunOperation {
            id: shotgunPush
            name: "shotgunPush"
            mode: ShotgunOperation.Push
            shotgun: shotgun
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
    
    Branch {
        id: prod
    	name: "root"
    	pattern: "/prod/projects/"

        ShotgunEntity {
            name: "HumanUser"
            
            ShotgunField {
                name: "login"
                field: "USER"
            }
        }
    }
    
    Branch {
        id: project
    	name: "project"
    	pattern: "{PROJECT}/"
        root: prod

        ShotgunEntity {
            name: "Project"
            
            ShotgunField {
                name: "name"
                field: "PROJECT"
            } 
        }
    }
    
    Transfer {
        id: transfer
        name: "transfer"
        root: project
    }
    
    Branch {
        id: scene
        name: "scene"
    	pattern: "shots/{SEQUENCE}/"
    	root: project

        ShotgunScene {
            project: project
        }
    }
    
    Branch {
        id: shot
        name: "shot"
    	pattern: "{SHOT}/"
    	root: scene

        ShotgunShot {
            project: project
            scene: scene
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
    
    Branch {
        id: asset
        name: "asset"
        pattern: "assets/{ASSET_TYPE}/{ASSET}/"
        root: project
    }
    
    Modeling {
        id: model
        name: "model"
        root: asset
    }
}