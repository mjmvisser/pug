import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Branch {
    name: "tracking"
    pattern: "3d/tracking/"
    
    Branch {
        id: release
        name: "release"
        pattern: "release/{VARIATION}/v{VERSION}"
        ReleaseOperation.versionField: findField("VERSION")

        ShotgunPublishEvent {
            project: node("/project")
            entityType: "Shot"
            entity: node("/shot")
            user: node("/prod")
            code: "{SEQUENCE}_{SHOT}_model_v{VERSION}"
        }

        File {
            id: release3deScene
            root: release
            name: "release3deScene"
            pattern: "3de/{SEQUENCE}_{SHOT}.3de"

            ShotgunFile {
                project: node("/project")
                release: release
                user: node("/prod")
            }
        }
        
        File {
            id: releaseNukeScript
            name: "releaseNukeScript"
            root: release
            pattern: "nk/{SEQUENCE}_{SHOT}_cam.nk"

            ShotgunFile {
                project: node("/project")
                release: release
                user: node("/prod")
            }
        }

        File {
            id: releaseMelScript
            name: "releaseMelScript"
            root: release
            pattern: "mel/{SEQUENCE}_{SHOT}_cam.mel"

            ShotgunFile {
                project: node("/project")
                release: release
                user: node("/prod")
            }
        }

        File {
            id: releaseLowUndist
            name: "releaseLowUndist"
            root: release
            pattern: "render/1280x675/jpg/{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_{VERSION}_1280x675.%04d.jpg"

            ShotgunFile {
                project: node("/project")
                release: release
                user: node("/prod")
                thumbnail: workLowThumbnail
                filmstrip: workLowFilmstrip
            }
        }

        File {
            id: releaseFullUndist
            name: "releaseFullUndist"
            root: release
            pattern: "render/2560x1350/jpg/{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_{VERSION}_2560x1350.%04d.jpg"

            ShotgunVersion {
                project: node("/project")
                entity { link: node("/shot"); linkType: "Shot" }
                release: release
                user: node("/prod")
                code: "{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_v{VERSION}"
                firstFrame: releaseSeq.details[releaseSeq.ShotgunOperation.index].element.firstFrame
                lastFrame: releaseSeq.details[releaseSeq.ShotgunOperation.index].element.lastFrame
                thumbnail: workFullThumbnail
                filmstrip: workFullFilmstrip
            }

            ShotgunFile {
                project: node("/project")
                release: release
                user: node("/prod")
                thumbnail: workFullThumbnail
                filmstrip: workFullFilmstrip
            }
        }
        
    }
    
    Branch {
        id: work
        name: "work"
        root: parentBranch
        pattern: "work/{USER}/"

        File {
            id: work3deScene
            name: "work3deScene"
            root: work
            pattern: "3de/{SHOT}.3de"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: release3deScene
        }
        
        File {
            id: workNukeScript
            name: "workNukeScript"
            root: work
            pattern: "exports/{SEQUENCE}_{SHOT}_cam.nk"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: releaseNukeScript
        }

        File {
            id: workMelScript
            name: "workMelScript"
            root: work
            pattern: "exports/{SEQUENCE}_{SHOT}_cam.nk"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: releaseMelScript
        }
        
        GenUndist {
            id: workLowUndist
            name: "workLowUndist"
            plateFile: node("../../plate/release/releaseSeq")
            nukeFile: workNukeScript
            format: "1k_und"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: releaseLowUndist
        }

        MakeThumbnail {
            id: workLowThumbnail
            name: "workLowThumbnail"
            input: workLowUndist
            filmstrip: false
        }

        MakeThumbnail {
            id: workLowFilmstrip
            name: "workLowFilmstrip"
            input: workLowUndist
            filmstrip: true
        }
        
        GenUndist {
            id: workFullUndist
            name: "workFullUndist"
            plateFile: node("../../plate/release/releaseSeq")
            nukeFile: workNukeScript
            format: "2k_und"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: releaseFullUndist
        }

        MakeThumbnail {
            id: workFullThumbnail
            name: "workFullThumbnail"
            input: workFullUndist
            filmstrip: false
        }

        MakeThumbnail {
            id: workFullFilmstrip
            name: "workFullFilmstrip"
            input: workFullUndist
            filmstrip: true
        }
    }
}
