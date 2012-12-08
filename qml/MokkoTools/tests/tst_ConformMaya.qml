import QtQuick 2.0
import QtTest 1.0
import Pug 1.0
import MokkoTools 1.0

TestCase {
    id: self
    name: "ConformMayaTests"

    property string testScenePath: Qt.resolvedUrl("data/scene.mb").replace("scene://", "")
    property string testImagePath: Qt.resolvedUrl("data/test.png").replace("file://", "")
        
    Root {
        operations: [
            UpdateOperation {
                id: update
            },
            CookOperation {
                id: cook
                dependencies: update
            }
        ]
        
        File {
            id: scene
            pattern: testScenePath
        }
        
        File {
            id: textures
            pattern: testImagePath 
        }
        
        ConformMaya {
            id: conform
            scene: scene
            textures: textures
        }
    }
    
    SignalSpy {
        id: spy
        target: cook
        signalName: "finished"
    }
    
    function test_Identify() {
        cook.run(conform, {});
        spy.wait(500);
        
        compare(scene.details.length, 1);
        compare(conform.CookOperation.status, Operation.Finished);
    }
}