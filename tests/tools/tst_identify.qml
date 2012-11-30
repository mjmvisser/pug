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
    
    Root {
        File {
            id: file
            pattern: "test.png"
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
    
    function test_identify() {
        cook.run(identify, {});
        spy.wait(500);
         
        compare(identify.CookOperation.status, Operation.Finished);
        compare(identify.elements[0].data.FORMAT, "82x104");
    }
}