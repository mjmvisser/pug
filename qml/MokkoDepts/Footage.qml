import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Branch {
    name: "footage"
    pattern: "2d/assets/footage"
    
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
            code: "{SCENE}_{SHOT}_footage_v{VERSION}"
        }

        File {
            id: releaseSeq
            name: "releaseSeq"
            pattern: "{FORMAT}/{EXT}/{SCENE}_{SHOT}_footage_{VARIATION}_v{VERSION}_{FORMAT}.{FRAME}.{EXT}"
            ShotgunOperation.action: ShotgunOperation.Create

            ShotgunVersion {
                project: node("/project")
                entity { link: node("/shot"); linkType: "Shot" }
                release: release
                user: node("/prod")
                code: "{SCENE}_{SHOT}_{STEP}_{VARIATION}_v{VERSION}"
                firstFrame: releaseSeq.details[releaseSeq.ShotgunOperation.index].element.firstFrame
                lastFrame: releaseSeq.details[releaseSeq.ShotgunOperation.index].element.lastFrame
                thumbnailPath: workSeq.thumbnail.details[releaseSeq.ShotgunOperation.index].element.path
                filmstripPath: workSeq.filmstrip.details[releaseSeq.ShotgunOperation.index].element.path
            }

            ShotgunFile {
                project: node("/project")
                release: release
                user: node("/prod")
                thumbnailPath: workSeqThumbnail.outputPath
                filmstripPath: workSeqfilmstrip.outputPath
            }
        }
    }

    Branch {
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