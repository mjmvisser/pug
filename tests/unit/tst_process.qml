import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    id: self
    name: "ProcessTests"

    UpdateOperation {
        id: update
    }
    
    CookOperation {
        id: cook
    }
    
    Root {
        Process {
            id: trueProcess
            count: 1
            argv: ["true"]
        }   
        
        Process {
            id: falseProcess
            count: 1
            argv: ["false"]
        }
        
        Process {
            logLevel: Log.Trace
            id: cookOnlyProcess
            count: 1
            argv: {
                if (cooking) {
                    return ["true"];
                } else {
                    return [];
                }
            }
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

    function test_processSuccess() {
        cookSpy.clear();
        cook.run(trueProcess, {});
        cookSpy.wait(500);
        compare(trueProcess.CookOperation.status, Operation.Finished);
        verify(trueProcess.details);
        compare(trueProcess.details[0].process.exitCode, 0);
    }

    function test_processFailure() {
        cookSpy.clear();
        console.log("Intentionally triggering process failure...");
        cook.run(falseProcess, {});
        cookSpy.wait(500);
        compare(falseProcess.CookOperation.status, Operation.Error);
        verify(falseProcess.details);
        compare(falseProcess.details[0].process.exitCode, 1);
    }
    
    function test_cookOnlyProcess() {
        updateSpy.clear();
        update.run(cookOnlyProcess, {});
        updateSpy.wait(500);
        compare(cookOnlyProcess.UpdateOperation.status, Operation.Finished);
    }
}