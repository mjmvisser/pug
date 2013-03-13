import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Folder {
    pattern: "3d/model/"
    
    fields: [
        Field { name: "LOD"; values: "master"; defaultValue: "master" }
    ]
    
    Folder {
        id: release
        name: "release"
        pattern: "release/{VARIATION}/v{VERSION}/"
        ReleaseOperation.versionField: "VERSION"
        ShotgunOperation.action: ShotgunOperation.Create
        
        ShotgunPublishEvent {
            id: sg_releasePublishEvent
            name: "sg_releasePublishEvent"
            project: node("/project/sg_project")
            entity: node("/asset/sg_asset")
            user: node("/sg_user")
            code: "{ASSET_TYPE}_{ASSET}_model_v{VERSION}"
        }
        
        File {
            id: releaseMayaMaster
            name: "releaseMayaMaster"
            pattern: "maya/master/bin/{ASSET}_{DEPARTMENT}_v{VERSION}.mb"
            ShotgunOperation.action: ShotgunOperation.Create
            
            ShotgunFile {
                id: sg_releaseMayaMasterFile
                name: "sg_releaseMayaMasterFile"
                project: node("/project/sg_project")
                release: sg_releasePublishEvent
                user: node("/sg_user")
            }
            
        }

        File {
            id: releaseMayaLod
            name: "releaseMayaLod"
            pattern: "maya/{LOD}/bin/{ASSET}_{DEPARTMENT}_{LOD}_v{VERSION}.mb"
            ShotgunOperation.action: ShotgunOperation.Create
            
            ShotgunFile {
                id: sg_releaseMayaLod
                name: "sg_releaseMayaLod"
                project: node("/project/sg_project")
                release: sg_releasePublishEvent
                user: node("/sg_user")
            }
            
        }

        // File {
            // id: releaseTurntableSeq
            // name: "releaseTurntableSeq"
            // pattern: "turntable/{LOD}/{ASSET_TYPE}_{ASSET}_{DEPARTMENT}_{VARIATION}_{LOD}_v{VERSION}.{FRAME}.{EXT}"
            // ShotgunOperation.action: ShotgunOperation.Create
//             
            // ShotgunVersion {
                // project: node("/project")
                // entityType: "Asset"
                // entity: node("/asset")
                // release: release
                // user: node("/prod")
                // code: "{ASSET_TYPE}_{ASSET}_{STEP}_{VARIATION}_v{VERSION}"
                // thumbnail: makeThumbnail
                // filmstrip: makeFilmstrip
            // }
        // }            
// 
        // File {
            // id: releaseTurntableMovie
            // name: "releaseTurntableMovie"
            // pattern: "turntable/{LOD}/{ASSET_TYPE}_{ASSET}_{DEPARTMENT}_{VARIATION}_{LOD}_v{VERSION}.{EXT}"
        // }
    }
    
    Folder {
        id: work
        name: "work"
        pattern: "work/{USER}/"

        fields: Field { name: "FILENAME" }
        
        File {
            name: "workScene"
            id: workMayaMaster
            pattern: "scenes/{FILENAME}.mb"
            ReleaseOperation.target: releaseMayaMaster 
            active: true
        }

        SplitMayaModel {
            name: "workLodScene"
            id: workMayaLod
            input: workMayaMaster
            ReleaseOperation.target: releaseMayaLod 
            active: true
        }
        
        // MakeTurntable {
            // id: makeTurntable
            // name: "makeTurntable"
            // scene: workMayaLod
            // ReleaseOperation.target: releaseTurntableSeq 
            // active: true
        // }
// 
        // MakeThumbnail {
            // id: makeThumbnail
            // name: "makeThumbnail"
            // input: makeTurntable
            // active: true
        // }
//     
        // MakeThumbnail {
            // id: makeFilmstrip
            // name: "makeFilmstrip"
            // input: makeTurntable
            // filmstrip: true
            // active: true
        // }
//         
        // MakeQuicktime {
            // id: makeMovie
            // name: "makeMovie"
            // input: makeTurntable
            // ReleaseOperation.target: releaseTurntableMovie 
        // }
    }
}