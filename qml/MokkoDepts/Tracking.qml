import QtQuick 2.0
import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Folder {
    pattern: "3d/tracking/"
    
    ShotgunStep {
        id: sg_step
        name: "sg_step"
        step: "tracking"
    }
    
    Folder {
        id: release
        name: "release"
        pattern: "release/{VARIATION}/v{VERSION}"
        ReleaseOperation.versionField: "VERSION"

        ShotgunVersion {
            id: sg_releaseFullUndistVersion
            name: "sg_releaseFullUndistVersion"
            project: node("/project/sg_project")
            link: node("/shot/sg_shot")
            step: sg_step
            user: node("/sg_user")
            code: "{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_v{VERSION}"
            thumbnail: workFullUndistThumbnail
            filmstrip: workFullUndistFilmstrip
        }

        File {
            id: release3deScene
            root: release
            name: "release3deScene"
            pattern: "3de/{SEQUENCE}_{SHOT}.3de"
            ReleaseOperation.source: work3deScene

            ShotgunPublishEvent {
                id: sg_release3deSceneFile
                name: "sg_release3deSceneFile"
                project: node("/project/sg_project")
                link: node("/shot/sg_shot")
                step: sg_step
                user: node("/sg_user")
            }
        }
        
        File {
            id: releaseNukeScript
            name: "releaseNukeScript"
            root: release
            pattern: "nk/{SEQUENCE}_{SHOT}_cam.nk"
            ReleaseOperation.source: workNukeScript

            ShotgunPublishEvent {
                id: sg_releaseNukeScriptFile
                name: "sg_releaseNukeScriptFile"
                project: node("/project/sg_project")
                link: node("/shot/sg_shot")
                step: sg_step
                user: node("/sg_user")
            }
        }

        File {
            id: releaseMelScript
            name: "releaseMelScript"
            root: release
            pattern: "mel/{SEQUENCE}_{SHOT}_cam.mel"
            ReleaseOperation.source: workMelScript

            ShotgunPublishEvent {
                id: sg_releaseMelScriptFile
                name: "sg_releaseMelScriptFile"
                project: node("/project/sg_project")
                link: node("/shot/sg_shot")
                step: sg_step
                user: node("/sg_user")
            }
        }

        File {
            id: releaseLowUndist
            name: "releaseLowUndist"
            root: release
            pattern: "render/1280x675/jpg/{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_{VERSION}_1280x675.%04d.jpg"
            ReleaseOperation.source: workLowUndist

            ShotgunPublishEvent {
                id: sg_releaseLowUndistFile
                name: "sg_releaseLowUndistFile"
                project: node("/project/sg_project")
                link: node("/shot/sg_shot")
                step: sg_step
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

            ShotgunPublishEvent {
                id: sg_releaseFullUndistFile
                name: "sg_releaseFullUndistFile"
                project: node("/project/sg_project")
                link: node("/shot/sg_shot")
                step: sg_step
                user: node("/sg_user")
                thumbnail: workFullUndistThumbnail
                filmstrip: workFullUndistFilmstrip
            }
        }
        
    }
    
    Folder {
        id: work
        name: "work"
        pattern: "work/{USER}/"

        File {
            id: work3deScene
            name: "work3deScene"
            root: work
            pattern: "3de/{SEQUENCE}_{SHOT}.3de"
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
            pattern: "exports/{SEQUENCE}_{SHOT}_cam.mel"
        }
        
        File {
            id: plateSeqRef
            input: node("/plate/release/plateReleaseSeq")
            context: {VERSION: sg_latestApprovedPlateVersion()}
        }
        
        LensDistort {
            id: workLowUndist
            name: "workLowUndist"
            input: plateSeqRef
            nukeScript: workNukeScript
            format: "low"
            mode: "undistort"
            active: true
        }
        
        MakeThumbnail {
            id: workLowUndistThumbnail
            name: "workLowUndistThumbnail"
            input: workLowUndist
            active: true
        }
        
        MakeThumbnail {
            id: workLowUndistFilmstrip
            name: "workLowUndistFilmstrip"
            input: workLowUndist
            filmstrip: true
            active: true
        }

        LensDistort {
            id: workFullUndist
            name: "workFullUndist"
            input: node("/plate/release/releaseSeq")
            nukeScript: workNukeScript
            format: "full"
            mode: "undistort"
            active: true
        }
        
        MakeThumbnail {
            id: workFullUndistThumbnail
            name: "workFullUndistThumbnail"
            input: workFullUndist
            active: true
        }
        
        MakeThumbnail {
            id: workFullUndistFilmstrip
            name: "workFullUndistFilmstrip"
            input: workFullUndist
            filmstrip: true
            active: true
        }
    }
}
