import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    name: "ProcessDetailTests"
    
    property string tmpDir: "/usr/tmp/pugunittests/"
    
    Root {
        //logLevel: Log.Info
        UpdateOperation {
            //logLevel: Log.Trace
            id: update
        }
        
        CookOperation {
            //logLevel: Log.Trace
            id: cook
            dependencies: update
        }

        Node {
            id: compound
            Process {
                //logLevel: Log.Trace
                //UpdateOperation.logLevel: Log.Trace
                id: writer
                name: "writer"
                count: 1
                argv: {
                    if (updating) {
                        return ['echo', '-e', 'begin-json details\n[{"mode": "update", "context": {"blah": "update"}}]\n===='];
                    } else if (cooking) {
                        return ['echo', '-e', 'begin-json details\n[{"mode": "cook", "context": {"blah": "cook"}}]\n===='];
                    } else {
                        return [];
                    }
                }
            }
            
            ProcessDetails {
                //logLevel: Log.Trace
                //UpdateOperation.logLevel: Log.Trace
                id: processor
                name: "processor"
                input: writer
                active: true
            }
            
            count: processor.count
            details: processor.details            
        }    
    }
    
    SignalSpy {
        id: updateSpy
        target: update
        signalName: "finished"
    }

    SignalSpy {
        id: cookSpy
        target: cook
        signalName: "finished"
    }
    
    function test_processDetailsUpdate() {
        updateSpy.clear();
        update.run(compound, {"foo": "bar"});
        updateSpy.wait(500);
       
        compare(update.status, Operation.Finished);       
        compare(compound.details.length, 1);
        compare(compound.details[0].context, {"foo": "bar", "blah": "update"})
        compare(compound.details[0].mode, "update")
    }
    
    function test_processDetailsCook() {
        cookSpy.clear();
        cook.run(compound, {"bat": "cat"});
        cookSpy.wait(500);
        compare(cookSpy.count, 1)
        
        compare(cook.status, Operation.Finished);       
        compare(compound.details.length, 1);
        compare(compound.details[0].context, {"bat": "cat", "blah": "cook"})
        compare(compound.details[0].mode, "cook")
    }
}