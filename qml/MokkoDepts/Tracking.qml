import QtQuick 2.0
import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

Folder {
    logLevel: Log.Error
    id: tracking
    pattern: "dept/{STEP}/"
    
    fields: [
        Field { name: "STEP"; values: "tracking" }
    ]
    
    FindShotgunStep {
        id: sg_step
        name: "sg_step"
        step: tracking
    }
    
    Folder {
        id: release
        name: "release"
        pattern: "release/{VARIATION}/v{VERSION}/"
        ReleaseOperation.versionField: "VERSION"

        outputs: [
            Output { name: "releases" }
        ]

        property list<Node> releases
        releases: [sg_releaseVersion, sg_release3deScene, sg_releaseNukeScript, 
                   sg_releaseMelScript, sg_releaseLowUndist, sg_releaseFullUndist]


        CreateShotgunVersion {
            id: sg_releaseVersion
            name: "sg_releaseVersion"
            sg_project: node("/project/sg_project")
            sg_entity: node("/shot/sg_shot")
            sg_step: sg_step
            sg_user: node("/sg_user")
            file: releaseFullUndist
            code: "{SEQUENCE}_{SHOT}_tracking_{VARIATION}_v{VERSION}"
            sg_publishEvent: sg_releaseFullUndist
        }

        File {
            id: release3deScene
            root: release
            name: "release3deScene"
            pattern: "3de/{SEQUENCE}_{SHOT}_{VARIATION}_v{VERSION}.3de"
            ReleaseOperation.source: work3deScene
            ReleaseOperation.versionBranch: release
        }

        CreateShotgunPublishEvent {
            id: sg_release3deScene
            name: "sg_release3deScene"
            sg_project: node("/project/sg_project")
            sg_entity: node("/shot/sg_shot")
            sg_step: sg_step
            sg_user: node("/sg_user")
            category: "source"
            file: release3deScene
            code: "{SEQUENCE}_{SHOT}_tracking_{VARIATION}_v{VERSION}_3de"
        }
        
        File {
            id: releaseNukeScript
            name: "releaseNukeScript"
            root: release
            pattern: "nk/{SEQUENCE}_{SHOT}_{VARIATION}_v{VERSION}_cam.nk"
            ReleaseOperation.source: workNukeScript
            ReleaseOperation.versionBranch: release
        }

        CreateShotgunPublishEvent {
            id: sg_releaseNukeScript
            name: "sg_releaseNukeScript"
            sg_project: node("/project/sg_project")
            sg_entity: node("/shot/sg_shot")
            sg_step: sg_step
            sg_user: node("/sg_user")
            sg_dependencies: sg_release3deScene
            category: "export"
            file: releaseNukeScript
            code: "{SEQUENCE}_{SHOT}_tracking_{VARIATION}_v{VERSION}_nk"
        }

        File {
            id: releaseMelScript
            name: "releaseMelScript"
            root: release
            pattern: "mel/{SEQUENCE}_{SHOT}_{VARIATION}_v{VERSION}_cam.mel"
            ReleaseOperation.source: workMelScript
            ReleaseOperation.versionBranch: release
        }

        CreateShotgunPublishEvent {
            id: sg_releaseMelScript
            name: "sg_releaseMelScript"
            sg_project: node("/project/sg_project")
            sg_entity: node("/shot/sg_shot")
            sg_step: sg_step
            sg_user: node("/sg_user")
            sg_dependencies: sg_release3deScene
            category: "export"
            file: releaseMelScript
            code: "{SEQUENCE}_{SHOT}_tracking_{VARIATION}_v{VERSION}_mel"
        }

        File {
            id: releaseLowUndist
            name: "releaseLowUndist"
            root: release
            pattern: "render/{RESOLUTION}/jpg/{SEQUENCE}_{SHOT}_tracking_{VARIATION}_{VERSION}_{RESOLUTION}.%04d.jpg"
            ReleaseOperation.source: workLowUndist
            ReleaseOperation.versionBranch: release
        }

        CreateShotgunPublishEvent {
            id: sg_releaseLowUndist
            name: "sg_releaseLowUndist"
            sg_project: node("/project/sg_project")
            sg_entity: node("/shot/sg_shot")
            sg_step: sg_step
            sg_user: node("/sg_user")
            sg_dependencies: [sg_releaseNukeScript, node("/plate/sg_currentPlate")]
            category: "render"
            file: releaseLowUndist
            code: "{SEQUENCE}_{SHOT}_tracking_{VARIATION}_v{VERSION}_{RESOLUTION}"
        }

        File {
            id: releaseFullUndist
            name: "releaseFullUndist"
            root: release
            pattern: "render/{RESOLUTION}/jpg/{SEQUENCE}_{SHOT}_tracking_{VARIATION}_{VERSION}_{RESOLUTION}.%04d.jpg"
            ReleaseOperation.source: workFullUndist
            ReleaseOperation.versionBranch: release
        }
        
        CreateShotgunPublishEvent {
            id: sg_releaseFullUndist
            name: "sg_releaseFullUndist"
            sg_project: node("/project/sg_project")
            sg_entity: node("/shot/sg_shot")
            sg_step: sg_step
            sg_user: node("/sg_user")
            sg_dependencies: [sg_releaseNukeScript, node("/plate/sg_currentPlate")]
            category: "render"
            file: releaseFullUndist
            code: "{SEQUENCE}_{SHOT}_tracking_{VARIATION}_v{VERSION}_{RESOLUTION}"
        }
    }
    
    Folder {
        id: work
        name: "work"
        pattern: "work/{USER}/"

        outputs: [
            Output { name: "work" }
        ]

        property list<Node> work
        work: [work3deScene, workNukeScript, workMelScript, workLowUndist, workFullUndist]

        File {
            id: work3deScene
            name: "work3deScene"
            root: work
            pattern: "scenes/{SEQUENCE}_{SHOT}.3de"
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
            inputs: Input { name: "sg_project" }
            property Node sg_project: node("/project/sg_project")
            name: "workLowUndist"
            input: node("/plate/sg_currentPlate")
            nukeScript: workNukeScript
            inputResolution: sg_project.details[0].entity.sg_image_resolution__in_
            outputResolution: sg_project.details[0].entity.sg_image_resolution__render_
            mode: "undistort"
        }
        
        LensDistort {
            id: workFullUndist
            inputs: Input { name: "sg_project" }
            property Node sg_project: node("/project/sg_project")
            name: "workFullUndist"
            input: node("/plate/sg_currentPlate")
            nukeScript: workNukeScript
            inputResolution: sg_project.details[0].entity.sg_image_resolution__low_
            outputResolution: sg_project.details[0].entity.sg_image_resolution__low_render_
            mode: "undistort"
        }
    }
}
