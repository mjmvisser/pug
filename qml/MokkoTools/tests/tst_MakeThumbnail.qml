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
        
        Folder {
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

    function test_seq() {
        spy.clear();
        cook.run(seq, {});
        spy.wait(100);

        var elementsView = Util.elementsView(seq);
        compare(elementsView.elements[0].frames.length, 10);
    }
    
    function test_makeThumbnail() {
        spy.clear();
        cook.run(makeThumbnail, {});
        spy.wait(5000);

        compare(cook.status, Operation.Finished)        
        compare(seq.details.length, 1);
        compare(makeThumbnail.details.length, 1);
    }
    
    function test_makeFilmstrip() {
        spy.clear();
        cook.run(makeFilmstrip, {});
        spy.wait(5000);

        compare(cook.status, Operation.Finished)        
        compare(seq.details.length, 1);
        compare(makeFilmstrip.details.length, 1);
    }
    
}
