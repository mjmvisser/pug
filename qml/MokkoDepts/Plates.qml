import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Folder {
    pattern: "2d/plate/"
    
    fields: [
        Field { name: "DEPARTMENT"; values: "plate" },
        Field { name: "EXT"; values: "dpx" }
    ]

    ShotgunStep {
        id: sg_step
        name: "sg_step"
        step: "plates"
    }

    Folder {
        id: release
        name: "release"
        pattern: "release/{VARIATION}/v{VERSION}/"
        ReleaseOperation.versionField: "VERSION"
        ShotgunOperation.action: ShotgunOperation.Create

        ShotgunVersion {
            id: sg_version
            name: "sg_version"
            project: node("/project/sg_project")
            link: node("/shot/sg_shot")
            step: sg_step
            user: node("/sg_user")
            code: "{SEQUENCE}_{SHOT}_comp_v{VERSION}"
            thumbnail: workSeqThumbnail
            filmstrip: workSeqFilmstrip
        }

        File {
            id: releaseSeq
            name: "plateReleaseSeq"
            pattern: "render/{FORMAT}/{EXT}/{SEQUENCE}_{SHOT}_{DEPARTMENT}_{VARIATION}_v{VERSION}_{FORMAT}.{FRAME}.{EXT}"
            ShotgunOperation.action: ShotgunOperation.Create
            ReleaseOperation.source: workSeq

            ShotgunPublishEvent {
                id: sg_releaseSeqFile
                name: "sg_releaseSeqFile"
                project: node("/project/sg_project")
                link: node("/shot/sg_shot")
                step: sg_step
                user: node("/sg_user")
                thumbnail: workSeqThumbnail
                filmstrip: workSeqFilmstrip
            }
        }
    }
    
    Folder {
        id: work
        name: "plateWork"
        pattern: "plateWork/{USER}/"

        File {
            id: workSeq
            name: "plateWorkSeq"
            pattern: "images/{SEQUENCE}_{SHOT}_{DEPARTMENT}.{FRAME}.{EXT}"
            active: true
        }

        MakeThumbnail {
            id: workSeqThumbnail
            name: "plateWorkSeqThumbnail"
            input: workSeq
        }
        
        MakeThumbnail {
            id: workSeqFilmstrip
            name: "plateWorkSeqFilmstrip"
            input: workSeq
            filmstrip: true
        }
        
    }
}
