import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Folder {
    pattern: "dept/plate/"
    
    fields: [
        Field { name: "DEPARTMENT"; values: "plate" },
        Field { name: "EXT"; values: "dpx" }
    ]

    ShotgunStep {
        id: sg_step
        name: "sg_step"
        step: "plate"
    }

    LatestShotgunPublishEvent {
        id: sg_currentPlate
        name: "sg_currentPlate"
        project: node("/project/sg_project")
        link: node("/shot/sg_shot")
        step: sg_step
    }
        

// 
    // Folder {
        // id: release
        // name: "release"
        // pattern: "release/{VARIATION}/v{VERSION}/"
        // ReleaseOperation.versionField: "VERSION"
// 
        // ShotgunVersion {
            // id: sg_version
            // action: ShotgunEntity.Create
            // name: "sg_version"
            // project: node("/project/sg_project")
            // link: node("/shot/sg_shot")
            // step: sg_step
            // user: node("/sg_user")
            // code: "{SEQUENCE}_{SHOT}_comp_v{VERSION}"
            // thumbnail: workSeqThumbnail
            // filmstrip: workSeqFilmstrip
        // }
// 
        // File {
            // id: releaseSeq
            // name: "plateReleaseSeq"
            // pattern: "render/{FORMAT}/{EXT}/{SEQUENCE}_{SHOT}_{DEPARTMENT}_{VARIATION}_v{VERSION}_{FORMAT}.{FRAME}.{EXT}"
            // ReleaseOperation.source: workSeq
// 
            // ShotgunPublishEvent {
                // id: sg_releaseSeqFile
                // action: ShotgunEntity.Create
                // name: "sg_releaseSeqFile"
                // project: node("/project/sg_project")
                // link: node("/shot/sg_shot")
                // step: sg_step
                // user: node("/sg_user")
                // thumbnail: workSeqThumbnail
                // filmstrip: workSeqFilmstrip
            // }
        // }
    // }
//     
    // Folder {
        // id: work
        // name: "plateWork"
        // pattern: "plateWork/{USER}/"
// 
        // File {
            // id: workSeq
            // name: "plateWorkSeq"
            // pattern: "images/{SEQUENCE}_{SHOT}_{DEPARTMENT}.{FRAME}.{EXT}"
            // active: true
        // }
// 
        // MakeThumbnail {
            // id: workSeqThumbnail
            // name: "plateWorkSeqThumbnail"
            // input: workSeq
        // }
//         
        // MakeThumbnail {
            // id: workSeqFilmstrip
            // name: "plateWorkSeqFilmstrip"
            // input: workSeq
            // filmstrip: true
        // }
//         
    // }
}
