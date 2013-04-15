import QtTest 1.0
import Pug 1.0
import MokkoTools 1.0

TestCase {
    property string testSequencePath: Qt.resolvedUrl("data/testSequence.%04d.exr").replace("file://", "")
    
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
        
        Folder {
            id: quicktimes
            name: "mokkotoolstests"
            pattern: "/usr/tmp/mokkotoolstests"

            File {
                id: seq
                name: "seq"
                pattern: testSequencePath
            }
    
            ShotgunQuicktime {
                id: mp4
                input: seq
                format: "mp4"
                output: true
            }
            
            ShotgunQuicktime {
                id: webm
                input: seq
                format: "webm"
                output: true
            }
        }
    }
    
    SignalSpy {
        id: spy
        target: cook
        signalName: "finished"
    }
    
    function test_undistorted() {
        cook.run(quicktimes, {"PUGWORK": "/usr/tmp/mokkotoolstests/"});
        spy.wait(25000);

        verify(seq.details.length == 1);
        verify(mp4.details.length == 1);
        verify(webm.details.length == 1);
    }
}
