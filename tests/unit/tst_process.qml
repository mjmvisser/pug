import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    id: self
    name: "ProcessTests"

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
    }
    
    SignalSpy {
        id: spy
        target: cook
        signalName: "finished"
    }

    function test_processSuccess() {
        spy.clear();
        cook.run(trueProcess, {});
        spy.wait(500);
        compare(trueProcess.CookOperation.status, Operation.Finished);
        verify(trueProcess.details);
        compare(trueProcess.details[0].process.exitCode, 0);
    }

    function test_processFailure() {
        spy.clear();
        console.log("Intentionally triggering process failure...");
        cook.run(falseProcess, {});
        spy.wait(500);
        compare(falseProcess.CookOperation.status, Operation.Error);
        verify(falseProcess.details);
        compare(falseProcess.details[0].process.exitCode, 1);
    }
}