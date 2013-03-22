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
        id: sg_delivery
        project: node("/project/sg_project")
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
            id: sg_release
            entity: sg_delivery
            project: node("/project/sg_project")
            user: node("/sg_user")
            code: "from_client_{TRANSFER}_v{VERSION}"
        }
        
        File {
            id: releaseFile
            name: "releaseFile"
            pattern: "{FILENAME}.{EXT}"
            ShotgunOperation.action: ShotgunOperation.Create
            ReleaseOperation.source: workFile

            ShotgunElement {
                code: "{FILENAME}.{EXT}"
                project: node("/project/sg_project")
                user: node("/sg_user")
                delivery: sg_delivery
                release: sg_release
            }
            
            ShotgunFile {
                project: node("/project/sg_project")
                release: sg_release
                user: node("/sg_user")
                thumbnail: workSeqThumbnail
            }
        }

        File {
            id: releaseSeq
            name: "releaseSeq"
            pattern: "{FILENAME}.{FRAME}.{EXT}"
            ShotgunOperation.action: ShotgunOperation.Create
            ReleaseOperation.source: workSeq

            ShotgunElement {
                project: node("/project/sg_project")
                user: node("/sg_user")
                delivery: sg_delivery
                release: sg_release
                code: "{FILENAME}.{FRAME}.{EXT}"
            }
            
            ShotgunFile {
                project: node("/project/sg_project")
                release: sg_release
                user: node("/sg_user")
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
