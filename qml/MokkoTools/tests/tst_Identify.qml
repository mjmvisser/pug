import QtQuick 2.0
import QtTest 1.0
import Pug 1.0
import MokkoTools 1.0

TestCase {
    id: self
    name: "IdentifyTests"

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
            id: file
            pattern: testImagePath
        }
        
        Identify {
            id: identify
            input: file
            metadata: {"FORMAT": "%[w]x%[h]"}
        }
    }
    
    SignalSpy {
        id: spy
        target: cook
        signalName: "finished"
    }
    
    function test_Identify() {
        cook.run(identify, {});
        spy.wait(500);
        
        compare(file.details.length, 1);
        compare(identify.CookOperation.status, Operation.Finished);
        compare(identify.details.length, 1);
        compare(identify.details[0].identify.FORMAT, "82x104");
    }
}