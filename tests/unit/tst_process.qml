import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    id: self
    name: "ProcessTests"
    
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
        id: detailsProcess
        count: 1
        argv: ["echo", "-e", 'random crap begin-json details\nbegin-json details crap\nbegin-json details\n[{"greeting": "hello", "language": "english"}, {"greeting": "bonjour", "language": "french"}]\n====\nrandom crap']
    }
     
    CookOperation {
        id: cook
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
        cook.run(falseProcess, {});
        spy.wait(500);
        compare(falseProcess.CookOperation.status, Operation.Error);
        verify(falseProcess.details);
        compare(falseProcess.details[0].process.exitCode, 1);
    }
    
    function test_processDetails() {
        spy.clear();
        cook.run(detailsProcess, {});
        spy.wait(500);
        compare(detailsProcess.CookOperation.status, Operation.Finished);
        compare(detailsProcess.details[0].language, "english");
        compare(detailsProcess.details[0].greeting, "hello");
        compare(detailsProcess.details[1].language, "french");
        compare(detailsProcess.details[1].greeting, "bonjour");
    }
}