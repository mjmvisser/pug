import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Branch {
    name: "plate"
    pattern: "2d/plate/"
    
    Field {
        name: "EXT"
        values: "dpx"
    }

    Branch {
        id: release
        name: "release"
        pattern: "release/{VARIATION}/{VERSION}/"
        ReleaseOperation.versionField: "VERSION"
        ShotgunOperation.action: ShotgunOperation.Create

        ShotgunPublishEvent {
            project: node("/project")
            entity { link: node("/shot"); linkType: "Shot" }
            user: node("/prod")
            code: "{SEQUENCE}_{SHOT}_comp_v{VERSION}"
        }

        Sequence {
            id: releaseSeq
            name: "releaseSeq"
            pattern: "render/{FORMAT}/{EXT}/{SCENE}_{SHOT}_{DEPARTMENT}_{VARIATION}_v{VERSION}_{FORMAT}.{FRAME}.{EXT}"
            ShotgunOperation.action: ShotgunOperation.Create

            ShotgunVersion {
                project: node("/project")
                entity { link: node("/shot"); linkType: "Shot" }
                release: release
                user: node("/prod")
                code: "{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_v{VERSION}"
                thumbnail: workSeq.thumbnail
                filmstrip: workSeq.filmstrip
            }

            ShotgunFile {
                project: node("/project")
                release: release
                user: node("/prod")
                thumbnail: workSeq.thumbnail
                filmstrip: workSeq.filmstrip
            }
        }
    }
    
    Branch {
        id: work
        name: "work"
        pattern: "work/{USER}/"

        Sequence {
            id: workSeq
            name: "workSeq"
            pattern: "images/{SCENE}_{SHOT}_{DEPARTMENT}.{FRAME}.{EXT}"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: releaseSeq
            output: true
        }
    }
}
