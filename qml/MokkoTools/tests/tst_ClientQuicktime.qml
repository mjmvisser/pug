import QtTest 1.0
import Pug 1.0
import MokkoTools 1.0

TestCase {
    property string testSequencePath: Qt.resolvedUrl("data/images/testSequence.{FRAME}.exr").replace("file://", "")
    property string testNukeTemplatePath: Qt.resolvedUrl("data/clientQuicktime.nk").replace("file://", "")
    
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
        
        ClientQuicktime {
            id: clientQuicktime
            name: "clientQuicktime"
            input: seq
            nukeTemplate: template
            resolution: "1920x1080"
            fps: 24
            codec: "DNxHD 115"
            filetype: "mov"
            project: "Project"
            sequence: "00"
            scene: "00_000"
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

    function test_clientQuicktime() {
        spy.clear();
        cook.run(clientQuicktime, {"PUGWORK": "/usr/tmp/mokkotoolstests/"});
        spy.wait(10000);

        compare(cook.status, Operation.Finished)        
        compare(seq.details.length, 1);
        compare(clientQuicktime.details.length, 1);
        
        verify(Util.exists(clientQuicktime.File.elements[0].path()));
    }
}
