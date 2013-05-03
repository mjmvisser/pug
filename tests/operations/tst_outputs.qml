import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "OutputTests"

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

        Node {
            id: folder
            name: "folder"
            property list<Node> nodes
            outputs: [
                Output { name: "nodes" }
            ]
            nodes: [nodeA, nodeB]
            
            Node {
                id: nodeA
                name: "nodeA"
                property bool failUpdate: context.nodeA.failUpdate
                property bool failCook: context.nodeA.failCook
                
                UpdateOperation.onCook: {
                    if (failUpdate) {
                        addError("failing update intentionally");
                    }
                    UpdateOperation.cookFinished();
                }
                
                CookOperation.onCook: {
                    if (failCook) {
                        addError("failing cook intentionally");
                    }
                    CookOperation.cookFinished();
                }
            }
            
            Node {
                id: nodeB
                name: "nodeB"
                property bool failUpdate: context.nodeB.failUpdate
                property bool failCook: context.nodeB.failCook
                
                UpdateOperation.onCook: {
                    if (failUpdate) {
                        addError("failing update intentionally");
                    }
                    UpdateOperation.cookFinished();
                }
                
                CookOperation.onCook: {
                    if (failCook) {
                        addError("failing cook intentionally");
                    }
                    CookOperation.cookFinished();
                }
            }
        }
    }
    
    SignalSpy {
        id: updateSpy
        signalName: "finished"
        target: update
    }

    SignalSpy {
        id: cookSpy
        signalName: "finished"
        target: cook
    }

    function test_update() {
        update.run(folder, {nodeA: {failUpdate: true, failCook: false},
                            nodeB: {failUpdate: false, failCook: false}});
        updateSpy.wait(500);
        
        compare(update.status, Operation.Error);
    }

    function test_cook() {
        cook.run(folder, {nodeA: {failUpdate: false, failCook: false},
                          nodeB: {failUpdate: false, failCook: true}});
        cookSpy.wait(500);
        
        compare(cook.status, Operation.Error);
    }
}