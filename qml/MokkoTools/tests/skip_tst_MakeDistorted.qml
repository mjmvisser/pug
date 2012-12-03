import QtTest 1.0
import Pug 1.0
import MokkoTools 1.0

TestCase {
    property string testSequencePath: Qt.resolvedUrl("data/testSequence.%04d.exr").replace("file://", "")
    property string testNukeDist: Qt.resolvedUrl("data/testNukeDist.nk").replace("file://", "")
    
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
        
        Branch {
            name: "mokkotoolstests"
            pattern: "/usr/tmp/mokkotoolstests"

            FileRef {
                id: seq
                name: "seq"
                pattern: testSequencePath
            }
    
            FileRef {
                name: "nukeDistortFile"
                id: nukeDistortFile
                pattern: testNukeDist
            }
    
            MakeDistorted {
                id: makeDist
                logLevel: Log.Debug
                UpdateOperation.logLevel: Log.Error
                CookOperation.logLevel: Log.Error
                input: seq
                nukeDistortFile: nukeDistortFile
                outputFormat: "100"
            }
        }
    }
    
    SignalSpy {
        id: spy
        target: cook
        signalName: "finished"
    }
    
    function test_makeDistorted() {
        cook.run(makeDist, {});
        spy.wait(25000);

        verify(seq.elements.length == 1);
        verify(makeDist.elements.length == 1);
        
    }
}
