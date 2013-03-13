import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Folder {
    name: "footage"
    pattern: "2d/assets/footage"
    
    Folder {
        id: release
        name: "release"
        pattern: "release/{VARIATION}/{VERSION}/"
        ReleaseOperation.versionField: "VERSION"
        ShotgunOperation.action: ShotgunOperation.Create

        ShotgunPublishEvent {
            id: sg_releasePublishEvent
            name: "sg_releasePublishEvent"
            project: node("/project/sg_project")
            entity: node("/shot/sg_shot")
            user: node("/sg_user")
            code: "{SEQUENCE}_{SHOT}_footage_v{VERSION}"
        }

        File {
            id: releaseSeq
            name: "releaseSeq"
            pattern: "{FORMAT}/{EXT}/{SEQUENCE}_{SHOT}_footage_{VARIATION}_v{VERSION}_{FORMAT}.{FRAME}.{EXT}"
            ShotgunOperation.action: ShotgunOperation.Create

            ShotgunVersion {
                project: node("/project/sg_project")
                entity: node("/shot/sg_shot")
                release: sg_releasePublishEvent
                user: node("/sg_user")
                code: "{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_v{VERSION}"
                thumbnailPath: workSeq.thumbnail
                filmstripPath: workSeq.filmstrip
            }

            ShotgunFile {
                project: node("/project/sg_project")
                release: sg_releasePublishEvent
                user: node("/sg_user")
                thumbnail: workSeq.thumbnail
                filmstrip: workSeq.filmstrip
            }
        }
    }

    Folder {
        id: work    
        name: "work"
        pattern: "work/{USER}/"
    
        Identify {
            id: readFormat
            input: node("/transfer/release/releaseSeq")
            metadata: {"FORMAT": "%[w]x%[h]"}
        }
    
        Sequence {
            id: workSeq
            input: readFormat
            linkType: File.Symbolic
            pattern: "{FILENAME}.{FRAME}.{EXT}"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: releaseSeq
        }
    }
}