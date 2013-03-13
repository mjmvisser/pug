import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    id: self
    name: "CoreTypeTests"
    
    Param {
        id: param
        name: "testParam"
        value: "testValue"
        text: "testText"
    }
    
    function test_operationEnums() {
        compare(Operation.Invalid, 0);
        compare(Operation.Finished, 1);
        compare(Operation.None, 2);
        compare(Operation.Idle, 3);
        compare(Operation.Running, 4);
        compare(Operation.Error, 5);
    }
    
    function test_param() {
        verify(param);
        verify(param.inherits("Param"));
        compare(param.className, "Param");
        compare(param.name, "testParam");
        compare(param.value, "testValue");
        compare(param.text, "testText");
    }

    DeprecatedNode {
        id: node
        name: "testNode"
    }
    
    function test_node() {
        verify(node);
        verify(node.inherits("DeprecatedNode"));
        compare(node.className, "DeprecatedNode");
        compare(node.name, "testNode");
        verify(node.node);
    }

    function test_util() {
        compare(Util.mkpath("foo/bar/baz"), true);
        compare(Util.rmdir("foo/bar/baz"), true);
        compare(Util.rmdir("foo/bar"), true);
        compare(Util.rmdir("foo"), true);
    }
}