import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Branch {
    pattern: "3d/model/"
    
    fields: [
        Field { name: "LOD"; values: "master"; defaultValue: "master" }
    ]
    
    Branch {
        id: release
        name: "release"
        pattern: "release/{VARIATION}/v{VERSION}"
        ReleaseOperation.versionField: "VERSION"
        ShotgunOperation.action: ShotgunOperation.Create
        
        ShotgunPublishEvent {
            project: node("/project")
            entityType: "Asset"
            entity: node("/asset")
            user: node("/prod")
            code: "{ASSET_TYPE}_{ASSET}_model_v{VERSION}"
        }
        
        File {
            id: releaseMaya
            name: "releaseMaya"
            pattern: "maya/{LOD}/{ASSET_TYPE}_{ASSET}_{DEPARTMENT}_{VARIATION}_{LOD}_v{VERSION}.mb"
            ShotgunOperation.action: ShotgunOperation.Create
            
            ShotgunFile {
                project: node("/project")
                release: release
                user: node("/prod")
            }
            
        }

        File {
            id: releaseZBrush
            name: "releaseZBrush"
            pattern: "zbrush/{ASSET_TYPE}_{ASSET}_{DEPARTMENT}_{VARIATION}_{LOD}_v{VERSION}.ztool"
            ShotgunOperation.action: ShotgunOperation.Create
            
            ShotgunFile {
                project: node("/project")
                release: release
                user: node("/prod")
            }
        }
        
        File {
            id: releaseTurntableSeq
            name: "releaseTurntableSeq"
            pattern: "turntable/{LOD}/{ASSET_TYPE}_{ASSET}_{DEPARTMENT}_{VARIATION}_{LOD}_v{VERSION}.{FRAME}.{EXT}"
            ShotgunOperation.action: ShotgunOperation.Create
            
            ShotgunVersion {
                project: node("/project")
                entityType: "Asset"
                entity: node("/asset")
                release: release
                user: node("/prod")
                code: "{ASSET_TYPE}_{ASSET}_{STEP}_{VARIATION}_v{VERSION}"
                thumbnail: workTurntableSeq.thumbnail
                filmstrip: workTurntableSeq.filmstrip
            }
        }            

        File {
            id: releaseTurntableMovie
            name: "releaseTurntableMovie"
            pattern: "turntable/{LOD}/{ASSET_TYPE}_{ASSET}_{DEPARTMENT}_{VARIATION}_{LOD}_v{VERSION}.{EXT}"
        }
    }
    
    Branch {
        id: work
        name: "work"
        pattern: "work/{USER}/"
        
        File {
            id: workMaya
            pattern: "scenes/{FILENAME}.mb"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: releaseMaya 
            output: true
        }

        File {
            id: workZBrush
            pattern: "zbrush/{FILENAME}.ztool"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: releaseMaya 
            output: true
        }

        MakeTurntable {
            id: makeTurntable
            name: "makeTurntable"
            input: workMaya
            ReleaseOperation.releasable: true
            ReleaseOperation.target: releaseTurntableSeq 
            output: true
        }

        MakeThumbnail {
            id: makeThumbnail
            name: "makeThumbnail"
            input: makeTurntable
            output: true
        }
    
        MakeThumbnail {
            id: makeFilmstrip
            name: "makeFilmstrip"
            input: makeTurntable
            filmstrip: true
            output: true
        }
        
        MakeQuicktime {
            id: makeMovie
            name: "makeMovie"
            input: makeTurntable
            ReleaseOperation.releasable: true
            ReleaseOperation.target: releaseTurntableMovie 
        }
    }
}