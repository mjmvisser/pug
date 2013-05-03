import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "OperationTests"

    Root {
        operations: [
            UpdateOperation {
                //logLevel: Log.Trace
                id: update
            },
            CookOperation {
                id: cook
                dependencies: update
            }
        ]

        Node {
            //UpdateOperation.logLevel: Log.Trace
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
            property Node input: nodeA
            
            inputs: [
                Input { name: "input" }
            ]
            
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

    function test_update_data() {
        return [
            {tag: "updateASuccess", 
             node: nodeA,
             context: {nodeA: {failUpdate: false, failCook: false},
                       nodeB: {failUpdate: false, failCook: false}},
             status: {update: {operation: Operation.Finished,
                               nodeA:  Operation.Finished}}},
            {tag: "updateAFailA", 
             node: nodeA,
             context: {nodeA: {failUpdate: true, failCook: false},
                       nodeB: {failUpdate: false, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error}}},
            {tag: "updateAFailB", 
             node: nodeA,
             context: {nodeA: {failUpdate: false, failCook: false},
                       nodeB: {failUpdate: true, failCook: false}},
             status: {update: {operation: Operation.Finished,
                               nodeA:  Operation.Finished}}},
            {tag: "updateAFailAB", 
             node: nodeA,
             context: {nodeA: {failUpdate: true, failCook: false},
                       nodeB: {failUpdate: true, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error}}},
            {tag: "updateBSuccess", 
             node: nodeB,
             context: {nodeA: {failUpdate: false, failCook: false},
                       nodeB: {failUpdate: false, failCook: false}},
             status: {update: {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Finished}}},
            {tag: "updateBFailA", 
             node: nodeB,
             context: {nodeA: {failUpdate: true, failCook: false},
                       nodeB: {failUpdate: false, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Error}}},
            {tag: "updateBFailB", 
             node: nodeB,
             context: {nodeA: {failUpdate: false, failCook: false},
                       nodeB: {failUpdate: true, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Error}}},
            {tag: "updateBFailAB", 
             node: nodeB,
             context: {nodeA: {failUpdate: true, failCook: false},
                       nodeB: {failUpdate: true, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Error}}},
        ]
    }

    function test_cook_data() {
        return [
            {tag: "cookASuccess", 
             node: nodeA,
             context: {nodeA: {failUpdate: false, failCook: false},
                       nodeB: {failUpdate: false, failCook: false}},
             status: {update: {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Invalid},
                      cook:   {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Invalid}}},
            {tag: "cookAFailUpdateA", 
             node: nodeA,
             context: {nodeA: {failUpdate: true, failCook: false},
                       nodeB: {failUpdate: false, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Invalid},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.Invalid}}},
            {tag: "cookAFailCookA", 
             node: nodeA,
             context: {nodeA: {failUpdate: false, failCook: true},
                       nodeB: {failUpdate: false, failCook: false}},
             status: {update: {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Invalid},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Invalid}}},
            {tag: "cookAFailUpdateACookA", 
             node: nodeA,
             context: {nodeA: {failUpdate: true, failCook: true},
                       nodeB: {failUpdate: false, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Invalid},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.Invalid}}},
            {tag: "cookAFailUpdateB", 
             node: nodeA,
             context: {nodeA: {failUpdate: false, failCook: false},
                       nodeB: {failUpdate: true, failCook: false}},
             status: {update: {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Invalid},
                      cook:   {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Invalid}}},
            {tag: "cookAFailCookB", 
             node: nodeA,
             context: {nodeA: {failUpdate: false, failCook: false},
                       nodeB: {failUpdate: false, failCook: true}},
             status: {update: {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Invalid},
                      cook:   {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Invalid}}},
            {tag: "cookAFailUpdateBCookB", 
             node: nodeA,
             context: {nodeA: {failUpdate: false, failCook: false},
                       nodeB: {failUpdate: true, failCook: true}},
             status: {update: {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Invalid},
                      cook:   {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Invalid}}},
            {tag: "cookAFailUpdateAUpdateB", 
             node: nodeA,
             context: {nodeA: {failUpdate: true, failCook: false},
                       nodeB: {failUpdate: true, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Invalid},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.Invalid}}},
            {tag: "cookAFailUpdateAUpdateBCookA", 
             node: nodeA,
             context: {nodeA: {failUpdate: true, failCook: true},
                       nodeB: {failUpdate: true, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Invalid},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.Invalid}}},
            {tag: "cookAFailUpdateAUpdateBCookB", 
             node: nodeA,
             context: {nodeA: {failUpdate: true, failCook: false},
                       nodeB: {failUpdate: true, failCook: true}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Invalid},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.Invalid}}},
            {tag: "cookAFailUpdateAUpdateBCookACookB", 
             node: nodeA,
             context: {nodeA: {failUpdate: true, failCook: true},
                       nodeB: {failUpdate: true, failCook: true}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Invalid},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.Invalid}}},
            {tag: "cookBSuccess", 
             node: nodeB,
             context: {nodeA: {failUpdate: false, failCook: false},
                       nodeB: {failUpdate: false, failCook: false}},
             status: {update: {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Finished},
                      cook:   {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Finished}}},
            {tag: "cookBFailUpdateA", 
             node: nodeB,
             context: {nodeA: {failUpdate: true, failCook: false},
                       nodeB: {failUpdate: false, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Error},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.None}}},
            {tag: "cookBFailCookA", 
             node: nodeB,
             context: {nodeA: {failUpdate: false, failCook: true},
                       nodeB: {failUpdate: false, failCook: false}},
             status: {update: {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Finished},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Error}}},
            {tag: "cookBFailUpdateACookA", 
             node: nodeB,
             context: {nodeA: {failUpdate: true, failCook: true},
                       nodeB: {failUpdate: false, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Error},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.None}}},
            {tag: "cookBFailUpdateB", 
             node: nodeB,
             context: {nodeA: {failUpdate: false, failCook: false},
                       nodeB: {failUpdate: true, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Error},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.None}}},
            {tag: "cookBFailCookB", 
             node: nodeB,
             context: {nodeA: {failUpdate: false, failCook: false},
                       nodeB: {failUpdate: false, failCook: true}},
             status: {update: {operation: Operation.Finished,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Finished},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Error}}},
            {tag: "cookBFailUpdateBCookB", 
             node: nodeB,
             context: {nodeA: {failUpdate: false, failCook: false},
                       nodeB: {failUpdate: true, failCook: true}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Finished,
                               nodeB:  Operation.Error},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.None}}},
            {tag: "cookBFailUpdateAUpdateB", 
             node: nodeB,
             context: {nodeA: {failUpdate: true, failCook: false},
                       nodeB: {failUpdate: true, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Error},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.None}}},
            {tag: "cookBFailUpdateAUpdateBCookA", 
             node: nodeB,
             context: {nodeA: {failUpdate: true, failCook: true},
                       nodeB: {failUpdate: true, failCook: false}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Error},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.None}}},
            {tag: "cookBFailUpdateAUpdateBCookB", 
             node: nodeB,
             context: {nodeA: {failUpdate: true, failCook: false},
                       nodeB: {failUpdate: true, failCook: true}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Error},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.None}}},
            {tag: "cookBFailUpdateAUpdateBCookACookB", 
             node: nodeB,
             context: {nodeA: {failUpdate: true, failCook: true},
                       nodeB: {failUpdate: true, failCook: true}},
             status: {update: {operation: Operation.Error,
                               nodeA:  Operation.Error,
                               nodeB:  Operation.Error},
                      cook:   {operation: Operation.Error,
                               nodeA:  Operation.None,
                               nodeB:  Operation.None}}}
        ]
    }

    property var nodes: {"update": {"operation": update,
                                    "nodeA": nodeA.UpdateOperation,
                                    "nodeB": nodeB.UpdateOperation},
                         "cook":   {"operation": cook,
                                    "nodeA": nodeA.CookOperation,
                                    "nodeB": nodeB.CookOperation}}
    
    function test_update(data) {
        update.run(data.node, data.context);
        updateSpy.wait(500);
        
        for (var op in data.status) {
            for (var node in data.status[op]) {
                if (nodes[op][node].status != data.status[op][node]) {
                    console.log("comparing nodes[" + op + "][" + node + "].status to data.status[" + op + "][" + node + "]:");
                }
                compare(nodes[op][node].status, data.status[op][node]);
            }
        }
    }   

    function test_cook(data) {
        cook.run(data.node, data.context);
        cookSpy.wait(500);
        
        for (var op in data.status) {
            for (var node in data.status[op]) {
                if (nodes[op][node].status != data.status[op][node]) {
                    console.log("comparing nodes[" + op + "][" + node + "].status to data.status[" + op + "][" + node + "]:");
                }
                compare(nodes[op][node].status, data.status[op][node]);
            }
        }
    }   

}