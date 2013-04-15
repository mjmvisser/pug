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
        
        File {
            id: seq
            name: "seq"
            pattern: testSequencePath
        }

        ShotgunThumbnail {
            name: "shotgunThumbnail"
            id: shotgunThumbnail
            input: seq
        }
        
        ShotgunThumbnail {
            name: "shotgunFilmstrip"
            id: shotgunFilmstrip
            input: seq
            filmstrip: true
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
    
    function test_shotgunThumbnail() {
        spy.clear();
        cook.run(shotgunThumbnail, {"PUGWORK": "/usr/tmp/mokkotoolstests/"});
        spy.wait(5000);

        compare(cook.status, Operation.Finished)        
        compare(seq.details.length, 1);
        compare(shotgunThumbnail.details.length, 1);
        
        var elementsView = Util.elementsView(shotgunThumbnail);
        verify(Util.exists(elementsView.elements[0].path()));
    }
    
    function test_shotgunFilmstrip() {
        spy.clear();
        cook.run(shotgunFilmstrip, {"PUGWORK": "/usr/tmp/mokkotoolstests/"});
        spy.wait(5000);

        compare(cook.status, Operation.Finished)        
        compare(seq.details.length, 1);
        compare(shotgunFilmstrip.details.length, 1);
        
        var elementsView = Util.elementsView(shotgunFilmstrip);
        verify(Util.exists(elementsView.elements[0].path()));
    }
    
}
