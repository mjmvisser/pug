import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "TestOperationTests"

    Root {
        operations: [
            TestOperation {
                id: testDependency
                name: "testDependency"
            },
            TestOperation {
                id: test
                name: "test"
                triggers: testTriggered
                dependencies: testDependency
            },
            TestOperation {
                id: testTriggered
                name: "testTriggered"
            }
        ]
        
        Branch {
            id: branchA
            name: "branchA"
            
            Branch {
                id: branchB
                name: "branchB"
                root: branchA
                
                Branch {
                    id: branchC
                    name: "branchC"
                    root: branchB
                }
            }
        }
    }
    
    SignalSpy {
        id: spy
        target: test
        signalName: "finished"
    }
    
    function test_depsAndTrigs() {
        test.run(branchC, {});
        spy.wait(1000);
        
        compare(branchA.TestOperation.status, Operation.Finished);
        compare(branchB.TestOperation.status, Operation.Finished);
        compare(branchC.TestOperation.status, Operation.Finished);

        compare(branchA.TestOperation.status, Operation.Finished);
        compare(branchB.TestOperation.status, Operation.Finished);
        compare(branchC.TestOperation.status, Operation.Finished);
        
        compare(branchA.TestOperation.status, Operation.Finished);
        compare(branchB.TestOperation.status, Operation.Finished);
        compare(branchC.TestOperation.status, Operation.Finished);
    }
}
                    
            
        