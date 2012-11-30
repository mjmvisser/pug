import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Branch {
    id: transfer
    pattern: "transfer/from_client/{TRANSFER}/"

    Field {
        name: "TRANSFER"
        regexp: "20\\d{6}"
    }

    Field {
        name: "FILENAME"
    }
    
    ShotgunDelivery {
        project: node("/project")
    }

    Branch {
        id: transferClient
        name: "transferClient"
        pattern: "client/"
    }
    
    Branch {
        id: release
        name: "release"
        pattern: "release/v{VERSION}/"
        ReleaseOperation.versionField: "VERSION"
        ShotgunOperation.action: ShotgunOperation.Create

        ShotgunPublishEvent {
            entity { link: transfer; linkType: "Delivery" }
            project: node("/project")
            user: node("/prod")
            code: "from_client_{TRANSFER}_v{VERSION}"
        }
        
        File {
            id: releaseFile
            name: "releaseFile"
            pattern: "{FILENAME}.{EXT}"
            ShotgunOperation.action: ShotgunOperation.Create

            ShotgunElement {
                code: "{FILENAME}.{EXT}"
                project: node("/project")
                user: node("/prod")
                delivery: transfer
                release: release
            }
            
            ShotgunFile {
                project: node("/project")
                release: release
                user: node("/prod")
                thumbnailPath: workSeqThumbnail.outputPath
            }
        }

        File {
            id: releaseSeq
            name: "releaseSeq"
            pattern: "{FILENAME}.{FRAME}.{EXT}"
            ShotgunOperation.action: ShotgunOperation.Create

            ShotgunElement {
                project: node("/project")
                user: node("/prod")
                delivery: transfer
                release: release
                code: "{FILENAME}.{FRAME}.{EXT}"
                startFrame: releaseSeq.element.firstFrame
                endFrame: releaseSeq.element.lastFrame
                sourcePathLink: releaseSeq           
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

        File {
            id: workFile
            name: "workFile"
            pattern: "{FILENAME}.{EXT}"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: releaseFile
        }

        MakeThumbnail {
            id: workFileThumbnail
            name: "workFileThumbnail"
            input: workFile
            output: true
        }

        File {
            id: workSeq
            name: "workFile"
            pattern: "{FILENAME}.{FRAME}.{EXT}"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: releaseSeq
        }
        
        MakeThumbnail {
            id: workSeqThumbnail
            name: "workSeqThumbnail"
            input: workSeq
            output: true
        }

        MakeThumbnail {
            id: workSeqfilmstrip
            name: "workSeqfilmstrip"
            input: workSeq
            filmstrip: true
            output: true
        }
    }
}
