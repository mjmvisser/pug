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
        
        Folder {
            name: "mokkotoolstests"
            pattern: "/usr/tmp/mokkotoolstests"

            File {
                id: seq
                name: "seq"
                pattern: testSequencePath
            }
    
            File {
                name: "nukeDistortFile"
                id: nukeDistortFile
                pattern: testNukeDist
            }
    
            LensDistort {
                id: undist
                input: seq
                nukeScript: nukeDistortFile
                format: "100"
                fileType: "jpg"
            }
        }
    }
    
    SignalSpy {
        id: spy
        target: cook
        signalName: "finished"
    }
    
    function test_undistorted() {
        cook.run(undist, {"PUGWORK": "/usr/tmp/mokkotoolstests/"});
        spy.wait(25000);

        verify(seq.details.length == 1);
        verify(undist.details.length == 1);
        
        var elementsView = Util.elementsView(undist);
        for (var i = 0; i < elementsView.elements[0].frames.length; i++) {
            verify(Util.exists(elementsView.element[0].frames[i].path()));
        }
        
    }
}
