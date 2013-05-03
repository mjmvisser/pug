import QtTest 1.0
import Pug 1.0
import MokkoTools 1.0

TestCase {
    property string testSequencePath: Qt.resolvedUrl("data/images/testSequence.{FRAME}.exr").replace("file://", "")
    property string testNukeTemplatePath: Qt.resolvedUrl("data/slate.nk").replace("file://", "")
    
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
        
        File {
            id: template
            name: "template"
            pattern: testNukeTemplatePath
        }
        
        NukeSlate {
            id: nukeSlate
            name: "nukeSlate"
            sequence: seq
            nukeTemplate: template
            format: "full"
            filetype: "exr"
            project: "Project"
            shot: "00_000_00"
            version: 1
            notes: "more cowbell"
            artist: "username"            
        }
    }
    
    SignalSpy {
        id: spy
        target: cook
        signalName: "finished"
    }

    function test_nukeSlate() {
        spy.clear();
        cook.run(nukeSlate, {"PUGWORK": "/usr/tmp/mokkotoolstests/"});
        spy.wait(10000);

        compare(cook.status, Operation.Finished)        
        compare(seq.details.length, 1);
        compare(nukeSlate.details.length, 1);
        
        var elementsView = Util.elementsView(nukeSlate);
        verify(Util.exists(elementsView.elements[0].path()));
    }
}
