import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Folder {
    name: "tracking"
    pattern: "3d/tracking/"
    
    Folder {
        id: release
        name: "release"
        pattern: "release/{VARIATION}/v{VERSION}"
        ReleaseOperation.versionField: "VERSION"

        ShotgunVersion {
            id: sg_releaseFullUndistVersion
            name: "sg_releaseFullUndistVersion"
            project: node("/project/sg_project")
            release: sg_releasePublishEvent
            user: node("/sg_user")
            code: "{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_v{VERSION}"
            thumbnail: workFullUndistThumbnail
            filmstrip: workFullUndistFilmstrip
        }

        ShotgunPublishEvent {
            id: sg_releasePublishEvent
            name: "sg_releasePublishEvent"
            project: node("/project/sg_project")
            entity: node("/shot/sg_shot")
            user: node("/sg_user")
            code: "{SEQUENCE}_{SHOT}_model_v{VERSION}"
        }

        File {
            id: release3deScene
            root: release
            name: "release3deScene"
            pattern: "3de/{SEQUENCE}_{SHOT}.3de"
            ReleaseOperation.source: work3deScene

            ShotgunFile {
                id: sg_release3deSceneFile
                name: "sg_release3deSceneFile"
                project: node("/project/sg_project")
                release: sg_releasePublishEvent
                user: node("/sg_user")
            }
        }
        
        File {
            id: releaseNukeScript
            name: "releaseNukeScript"
            root: release
            pattern: "nk/{SEQUENCE}_{SHOT}_cam.nk"
            ReleaseOperation.source: workNukeScript

            ShotgunFile {
                id: sg_releaseNukeScriptFile
                name: "sg_releaseNukeScriptFile"
                project: node("/project/sg_project")
                release: sg_releasePublishEvent
                user: node("/sg_user")
            }
        }

        File {
            id: releaseMelScript
            name: "releaseMelScript"
            root: release
            pattern: "mel/{SEQUENCE}_{SHOT}_cam.mel"
            ReleaseOperation.source: workMelScript

            ShotgunFile {
                id: sg_releaseMelScriptFile
                name: "sg_releaseMelScriptFile"
                project: node("/project/sg_project")
                release: sg_releasePublishEvent
                user: node("/sg_user")
            }
        }

        File {
            id: releaseLowUndist
            name: "releaseLowUndist"
            root: release
            pattern: "render/1280x675/jpg/{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_{VERSION}_1280x675.%04d.jpg"
            ReleaseOperation.source: workLowUndist

            ShotgunFile {
                id: sg_releaseLowUndistFile
                name: "sg_releaseLowUndistFile"
                project: node("/project/sg_project")
                release: sg_releasePublishEvent
                user: node("/sg_user")
                thumbnail: workLowUndistThumbnail
                filmstrip: workLowUndistFilmstrip
            }
        }

        File {
            id: releaseFullUndist
            name: "releaseFullUndist"
            root: release
            pattern: "render/2560x1350/jpg/{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_{VERSION}_2560x1350.%04d.jpg"
            ReleaseOperation.source: workFullUndist

            ShotgunFile {
                id: sg_releaseFullUndistFile
                name: "sg_releaseFullUndistFile"
                project: node("/project/sg_project")
                release: sg_releasePublishEvent
                user: node("/sg_user")
                thumbnail: workFullUndistThumbnail
                filmstrip: workFullUndistFilmstrip
            }
        }
        
    }
    
    Folder {
        id: work
        name: "work"
        root: parentFolder
        pattern: "work/{USER}/"

        File {
            id: work3deScene
            name: "work3deScene"
            root: work
            pattern: "3de/{SHOT}.3de"
        }
        
        File {
            id: workNukeScript
            name: "workNukeScript"
            root: work
            pattern: "exports/{SEQUENCE}_{SHOT}_cam.nk"
        }

        File {
            id: workMelScript
            name: "workMelScript"
            root: work
            pattern: "exports/{SEQUENCE}_{SHOT}_cam.nk"
        }
        
        LensDistort {
            id: workLowUndist
            name: "workLowUndist"
            plateFile: node("../../plate/release/releaseSeq")
            nukeFile: workNukeScript
            format: "low"
            mode: "undistort"
        }
        
        MakeThumbnail {
            id: workLowUndistThumbnail
            name: "workLowUndistThumbnail"
            input: workLowUndist
        }
        
        MakeFilmstrip {
            id: workLowUndistFilmstrip
            name: "workLowUndistFilmstrip"
            input: workLowUndist
        }

        GenUndist {
            id: workFullUndist
            name: "workFullUndist"
            plateFile: node("../../plate/release/releaseSeq")
            nukeFile: workNukeScript
            format: "full"
            mode: "undistort"
        }
        
        MakeThumbnail {
            id: workFullUndistThumbnail
            name: "workFullUndistThumbnail"
            input: workFullUndist
        }
        
        MakeThumbnail {
            id: workFullUndistFilmstrip
            name: "workFullUndistFilmstrip"
            input: workFullUndist
            filmstrip: true
        }
    }
}
