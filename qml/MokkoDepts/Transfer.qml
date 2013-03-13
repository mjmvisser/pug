import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Folder {
    id: transfer
    pattern: "transfer/from_client/{TRANSFER}/"

    fields: [
        Field { name: "TRANSFER"; regexp: "20\\d{6}" },
        Field { name: "FILENAME" }
    ]
    
    ShotgunDelivery {
        project: node("/project")
    }

    Folder {
        id: transferClient
        name: "transferClient"
        pattern: "client/"
    }
    
    Folder {
        id: release
        name: "release"
        pattern: "release/v{VERSION}/"
        ReleaseOperation.versionField: "VERSION"
        ShotgunOperation.action: ShotgunOperation.Create

        ShotgunPublishEvent {
            entityType: "Delivery"
            entity: transfer
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
                thumbnail: workSeqThumbnail
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
            }
            
            ShotgunFile {
                project: node("/project")
                release: release
                user: node("/prod")
                thumbnail: workSeqThumbnail
                filmstrip: workSeqfilmstrip
            }
        }
    }

    Folder {
        id: work
        name: "work"
        pattern: "work/{USER}/"

        File {
            id: workFile
            name: "workFile"
            pattern: "{FILENAME}.{EXT}"
            ReleaseOperation.target: releaseFile
        }

        MakeThumbnail {
            id: workFileThumbnail
            name: "workFileThumbnail"
            input: workFile
            active: true
        }

        File {
            id: workSeq
            name: "workSeq"
            pattern: "{FILENAME}.{FRAME}.{EXT}"
            ReleaseOperation.target: releaseSeq
        }
        
        MakeThumbnail {
            id: workSeqThumbnail
            name: "workSeqThumbnail"
            input: workSeq
            active: true
        }

        MakeThumbnail {
            id: workSeqfilmstrip
            name: "workSeqfilmstrip"
            input: workSeq
            filmstrip: true
            active: true
        }
    }
}
