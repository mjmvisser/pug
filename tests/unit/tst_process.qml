import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    id: self
    name: "ProcessTests"
    
    Process {
        logLevel: Log.Warning
        id: trueProcess
        argv: ["true"]
        CookOperation.cookable: true
    }   
    
    Process {
        logLevel: Log.Warning
        id: falseProcess
        argv: ["false"]
        CookOperation.cookable: true
    }
     
    CookOperation {
        logLevel: Log.Warning
        id: cook
    }
    
    SignalSpy {
        id: spy
        target: cook
        signalName: "finished"
    }

    function test_processSuccess() {
        verify(trueProcess);
        verify(trueProcess.inherits("Process"));
        compare(trueProcess.className, "Process");
        spy.clear();
        compare(spy.count, 0);
        cook.run(trueProcess, {});
        verify(trueProcess.CookOperation.status == Operation.Idle || 
               trueProcess.CookOperation.status == Operation.Running);
        spy.wait(500);
        compare(spy.count, 1);
        compare(trueProcess.CookOperation.status, Operation.Finished);
    }

    function test_processFailure() {
        verify(falseProcess);
        verify(falseProcess.inherits("Process"));
        compare(falseProcess.className, "Process");
        spy.clear();
        compare(spy.count, 0);
        cook.run(falseProcess, {});
        verify(falseProcess.CookOperation.status == Operation.Idle || 
               falseProcess.CookOperation.status == Operation.Running);
        spy.wait(500);
        compare(spy.count, 1);
        compare(falseProcess.CookOperation.status, Operation.Error);
    }
}