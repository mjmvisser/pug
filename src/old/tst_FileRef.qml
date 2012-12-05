import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    UpdateOperation {
        logLevel: Log.Debug
        id: update
    }
    
    property string testSequencePath: Qt.resolvedUrl("data/testSequence.%04d.exr").replace("file://", "") 
    
    FileRef {
        id: seq
        name: "testSequence"
        pattern: testSequencePath
    }
    
    SignalSpy {
        id: spy
        target: update
        signalName: "finished"
    }
    
    function test_fileRefSeq() {
        update.run(seq, {});
        spy.wait();
        
        compare(seq.details[0].element.pattern, testSequencePath);
        compare(seq.details[0].element.firstFrame, 1);
        compare(seq.details[0].element.lastFrame, 10);
    }
}
