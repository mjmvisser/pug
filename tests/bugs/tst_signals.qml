import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    name: "BugTests"
    
    ExtraFancyWidget {
        id: efw
    }
    
    SignalSpy {
        id: efwSpy
        target: efw
        signalName: "updated"
    }
    
    function test_efb() {
        efw.fancyUpdateText({text: "FOO"});
        efwSpy.wait(500);
        compare(efw.text, "FOO");
    }
    
    MyFancyNode {
        id: fancyNode
    }
    
    SignalSpy {
        id: fancyNodeSpy
        target: fancyNode
        signalName: "released"
    }
    
    function test_myFancyNode() {
        fancyNode.release({FOO: "foo"});
        fancyNodeSpy.wait(500);
        compare(fancyNode.foo, "foo");
    }
}