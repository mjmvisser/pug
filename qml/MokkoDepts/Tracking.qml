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

        File {
            root: release
            name: "3deScene"
            pattern: "3de/{SEQUENCE}_{SHOT}.3de"
        }
        
        File {
            name: "nukeScript"
            root: release
            pattern: "nk/{SEQUENCE}_{SHOT}_cam.nk"
        }

        File {
            name: "melScript"
            root: release
            pattern: "mel/{SEQUENCE}_{SHOT}_cam.mel"
        }

        File {
            name: "lowUndist"
            root: release
            pattern: "images/1k_und/jpg/{SEQUENCE_SHOT}_1k_und.%04d.jpg"
        }
        
    }
    
    Branch {
        id: work
        name: "work"
        root: parentBranch
        pattern: "work/{USER}/"

        File {
            root: work
            pattern: "3de/{SHOT}.3de"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: node("../../work/3deScript")
        }
        
        File {
            root: work
            pattern: "exports/{SEQUENCE}_{SHOT}_cam.nk"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: node("../../release/nukeScript")
        }

        File {
            id: workMelFile
            root: work
            pattern: "exports/{SEQUENCE}_{SHOT}_cam.nk"
            ReleaseOperation.releasable: true
            ReleaseOperation.target: node("../../release/melScript")
        }
        
        // FileRef {
            // id: plateFile
            // target: "../plate/release/plate"
        // }
        
        // GenUndist {
            // id: genUndist
            // plateFile: plateFile
            // nukeFile: nukeNodeFile
            // format: "1k_und" 
        // }
        
        // File {
            // root: work
            // input: genUndist
            // pattern: "images/low_und/{SHOT}_1k_und.%04d.jpg"
            // ReleaseOperation.releasable: true
            // ReleaseOperation.target: node("../../release/lowUndist")
        // }
    }
}
