import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Folder {
    name: "plate"
    pattern: "2d/plate/"
    
    fields: Field { name: "EXT"; values: "dpx" }

    Folder {
        id: release
        name: "release"
        pattern: "release/{VARIATION}/{VERSION}/"
        ReleaseOperation.versionField: "VERSION"
        ShotgunOperation.action: ShotgunOperation.Create

        ShotgunPublishEvent {
            id: sg_release
            name: "sg_release"
            project: node("/project/sg_project")
            entity: node("/shot/sg_shot")
            user: node("/sg_user")
            code: "{SEQUENCE}_{SHOT}_comp_v{VERSION}"
        }

        File {
            id: releaseSeq
            name: "releaseSeq"
            pattern: "render/{FORMAT}/{EXT}/{SEQUENCE}_{SHOT}_{DEPARTMENT}_{VARIATION}_v{VERSION}_{FORMAT}.{FRAME}.{EXT}"
            ShotgunOperation.action: ShotgunOperation.Create
            ReleaseOperation.source: workSeq

            ShotgunVersion {
                id: sg_releaseSeqVersion
                name: "sg_releaseSeqVersion"
                project: node("/project/sg_project")
                entity: node("/shot/sg_shot")
                release: sg_release
                user: node("/sg_user")
                code: "{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_v{VERSION}"
                thumbnail: workSeqThumbnail
                filmstrip: workSeqFilmstrip
            }

            ShotgunFile {
                id: sg_releaseSeqFile
                name: "sg_releaseSeqFile"
                project: node("/project/sg_project")
                release: sg_release
                user: node("/sg_user")
                thumbnail: workSeqThumbnail
                filmstrip: workSeqFilmstrip
            }
        }
    }
    
    Folder {
        id: work
        name: "work"
        pattern: "work/{USER}/"

        File {
            id: workSeq
            name: "workSeq"
            pattern: "images/{SEQUENCE}_{SHOT}_{DEPARTMENT}.{FRAME}.{EXT}"
            active: true
        }

        MakeThumbnail {
            id: workSeqThumbnail
            name: "workSeqThumbnail"
            input: workSeq
        }
        
        MakeThumbnail {
            id: workSeqFilmstrip
            name: "workSeqFilmstrip"
            input: workSeq
            filmstrip: true
        }
        
    }
}
