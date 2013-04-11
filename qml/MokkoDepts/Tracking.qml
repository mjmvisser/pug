import QtQuick 2.0
import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Folder {
    pattern: "dept/tracking/"
    
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
            action: ShotgunEntity.Create
            name: "sg_releaseFullUndistVersion"
            project: node("/project/sg_project")
            link: node("/shot/sg_shot")
            step: sg_step
            user: node("/sg_user")
            code: "{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_v{VERSION}"
            thumbnail: workUndistThumbnail
            filmstrip: workUndistFilmstrip
        }

        File {
            id: release3deScene
            root: release
            name: "release3deScene"
            pattern: "3de/{SEQUENCE}_{SHOT}.3de"
            ReleaseOperation.source: work3deScene

            ShotgunPublishEvent {
                id: sg_release3deScene
                action: ShotgunEntity.Create
                name: "sg_release3deScene"
                project: node("/project/sg_project")
                link: node("/shot/sg_shot")
                step: sg_step
                user: node("/sg_user")
                category: "source"
                code: "{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_v{VERSION}"
            }
        }
        
        File {
            id: releaseNukeScript
            name: "releaseNukeScript"
            root: release
            pattern: "nk/{SEQUENCE}_{SHOT}_cam.nk"
            ReleaseOperation.source: workNukeScript

            ShotgunPublishEvent {
                id: sg_releaseNukeScript
                name: "sg_releaseNukeScript"
                project: node("/project/sg_project")
                link: node("/shot/sg_shot")
                step: sg_step
                user: node("/sg_user")
                category: "nk"
                dependencies: sg_release3deScene
            }
        }

        File {
            id: releaseMelScript
            name: "releaseMelScript"
            root: release
            pattern: "mel/{SEQUENCE}_{SHOT}_cam.mel"
            ReleaseOperation.source: workMelScript

            ShotgunPublishEvent {
                id: sg_releaseMelScript
                name: "sg_releaseMelScript"
                project: node("/project/sg_project")
                link: node("/shot/sg_shot")
                step: sg_step
                user: node("/sg_user")
                category: "mel"
                dependencies: sg_release3deScene
            }
        }

        File {
            id: releaseLowUndist
            name: "releaseLowUndist"
            root: release
            pattern: "render/1120x630/jpg/{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_{VERSION}_1120x630.%04d.jpg"
            ReleaseOperation.source: workLowUndist

            ShotgunPublishEvent {
                id: sg_releaseLowUndist
                name: "sg_releaseLowUndist"
                project: node("/project/sg_project")
                link: node("/shot/sg_shot")
                step: sg_step
                user: node("/sg_user")
                resolution: "1120x630"
                category: "render"
                thumbnail: workUndistThumbnail
                filmstrip: workUndistFilmstrip
                dependencies: [sg_releaseNukeScript, workLowUndist.input]
            }
        }

        File {
            id: releaseFullUndist
            name: "releaseFullUndist"
            root: release
            pattern: "render/2240x1260/jpg/{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_{VERSION}_2240x1260.%04d.jpg"
            ReleaseOperation.source: workFullUndist

            ShotgunPublishEvent {
                id: sg_releaseFullUndist
                name: "sg_releaseFullUndist"
                project: node("/project/sg_project")
                link: node("/shot/sg_shot")
                step: sg_step
                user: node("/sg_user")
                resolution: "2240x1260"
                category: "render"
                thumbnail: workUndistThumbnail
                filmstrip: workUndistFilmstrip
                dependencies: [sg_releaseNukeScript, workLowUndist.input]
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

        LensDistort {
            id: workLowUndist
            name: "workLowUndist"
            input: node("/plate/sg_currentPlate")
            nukeScript: workNukeScript
            format: "low"
            mode: "undistort"
            output: true
        }
        
        LensDistort {
            id: workFullUndist
            name: "workFullUndist"
            input: node("/plate/sg_currentPlate")
            nukeScript: workNukeScript
            format: "full"
            mode: "undistort"
            output: true
        }
        
        MakeThumbnail {
            id: workUndistThumbnail
            name: "workUndistThumbnail"
            input: workLowUndist
            output: true
        }
        
        MakeThumbnail {
            id: workUndistFilmstrip
            name: "workUndistFilmstrip"
            input: workLowUndist
            filmstrip: true
            output: true
        }
    }
}
