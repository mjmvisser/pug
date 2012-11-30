import QtQuick 2.0
import QtTest 1.0
import Pug 1.0
import MokkoTools 1.0

TestCase {
    id: self
    name: "IdentifyTests"

    UpdateOperation {
        id: update
    }

    CookOperation {
        id: cook
        dependencies: update
    }
    
    property string testImagePath: Qt.resolvedUrl("data/test.png").replace("file://", "")
    
    Root {
        FileRef {
            id: file
            pattern: testImagePath
        }
        
        Identify {
            id: identify
            // logLevel: Log.Debug
            // UpdateOperation.logLevel: Log.Error
            // CookOperation.logLevel: Log.Error
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
        
        compare(file.elements.length, 1);
        compare(identify.CookOperation.status, Operation.Finished);
        compare(identify.elements[0].pattern, testImagePath);
        compare(identify.elements[0].data.FORMAT, "82x104");
    }
}