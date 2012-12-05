import QtTest 1.0
import Pug 1.0
import MokkoTools 1.0

TestCase {
    property string testSequencePath: Qt.resolvedUrl("data/testSequence.{FRAME}.exr").replace("file://", "")
    
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
        
        fields: FrameSpecField { name: "FRAME" }
        
        Branch {
            name: "mokkotoolstests"
            pattern: "/usr/tmp/mokkotoolstests"

            File {
                id: seq
                name: "seq"
                pattern: testSequencePath
            }
    
            MakeThumbnail {
                name: "makeThumbnail"
                id: makeThumbnail
                input: seq
            }
            
            MakeThumbnail {
                name: "makeFilmstrip"
                id: makeFilmstrip
                input: seq
                filmstrip: true
            }
        }
    }
    
    SignalSpy {
        id: spy
        target: cook
        signalName: "finished"
    }
    
    function test_makeThumbnail() {
        cook.run(makeThumbnail, {});
        spy.wait(25000);

        compare(cook.status, Operation.Finished)        
        verify(seq.details.length == 1);
        verify(makeThumbnail.details.length == 1);
    }
    
    function test_makeFilmstrip() {
        cook.run(makeFilmstrip, {});
        spy.wait(25000);

        compare(cook.status, Operation.Finished)        
        compare(seq.details.length, 1);
        compare(makeFilmstrip.details.length, 1);
    }
    
}
