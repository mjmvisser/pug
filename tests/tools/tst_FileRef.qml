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
        logLevel: Log.Debug
        UpdateOperation.logLevel: Log.Debug
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
        
        compare(seq.elements[0].pattern, testSequencePath);
        compare(seq.elements[0].firstFrame, 1);
        compare(seq.elements[0].lastFrame, 10);
    }
}
