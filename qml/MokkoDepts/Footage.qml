import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Folder {
    name: "footage"
    pattern: "dept/assets/footage"

    ShotgunStep {
        id: sg_step
        name: "sg_step"
        step: "footage"
    }
    
    Folder {
        id: release
        name: "release"
        pattern: "release/{VARIATION}/{VERSION}/"
        ReleaseOperation.versionField: "VERSION"
        ShotgunOperation.action: ShotgunOperation.Create

        ShotgunVersion {
            id: sg_releaseVersion
            name: "sg_releaseVersion"
            project: node("/project/sg_project")
            entity: null //node("/shot/sg_shot")
            step: sg_step
            user: node("/sg_user")
            code: "{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_v{VERSION}"
            thumbnailPath: workSeq.thumbnail
            filmstripPath: workSeq.filmstrip
        }

        File {
            id: releaseSeq
            name: "releaseSeq"
            pattern: "{FORMAT}/{EXT}/{SEQUENCE}_{SHOT}_footage_{VARIATION}_v{VERSION}_{FORMAT}.{FRAME}.{EXT}"
            ShotgunOperation.action: ShotgunOperation.Create

            ShotgunPublishEvent {
                project: node("/project/sg_project")
                entity: null //node("/shot/sg_shot")
                step: sg_step
                user: node("/sg_user")
                thumbnailPath: workSeq.thumbnail
                filmstripPath: workSeq.filmstrip
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