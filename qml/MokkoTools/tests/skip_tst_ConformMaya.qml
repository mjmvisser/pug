import QtQuick 2.0
import QtTest 1.0
import Pug 1.0
import MokkoTools 1.0

TestCase {
    id: self
    name: "ConformMayaTests"

    property string testScenePath: Qt.resolvedUrl("data/scene.mb").replace("file://", "")
    property string releaseScenePath: Qt.resolvedUrl("data/release_scene.mb").replace("file://", "")
    property string testImagePath: Qt.resolvedUrl("data/test.png").replace("file://", "")
    property string releaseImagePath: Qt.resolvedUrl("data/release_test.png").replace("file://", "")
        
    Root {
        operations: [
            UpdateOperation {
                id: update
            },
            CookOperation {
                id: cook
                dependencies: update
            },
            ReleaseOperation {
                id: release
                dependencies: update
            }
        ]

        File {
            id: releaseScene
            name: "releaseScene"
            pattern: releaseScenePath 
        }
        
        File {
            id: releaseTextures
            name: "releaseTextures"
            pattern: releaseImagePath 
        }

        File {
            id: scene
            name: "scene"
            pattern: testScenePath
            ReleaseOperation.target: releaseScene
        }
        
        File {
            id: refs
            name: "refs"
            pattern: testImagePath
            ReleaseOperation.target: releaseTextures 
        }
        
        ConformMaya {
            id: conform
            name: "conform"
            scene: scene
            refs: refs
            
            Component.onCompleted: {
                env["MOKKO_WORK_TYPE"] = "asset"
                env["MOKKO_DEPARTMENT"] = "modeling"
            }
            
        }
    }
    
    SignalSpy {
        id: spy
        target: cook
        signalName: "finished"
    }
    
    function test_Conform() {
        cook.run(conform, {});
        spy.wait(30000);
        
        compare(scene.details.length, 1);
        compare(conform.CookOperation.status, Operation.Finished);
    }
}