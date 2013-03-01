import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "TractorOperationTests"

    function init() {
        Util.mkpath(tmpDir + "tractortests/abc/foo/work");
        Util.touch(tmpDir + "tractortests/abc/foo/work/baa.txt");
        Util.touch(tmpDir + "tractortests/abc/foo/work/bar.txt");
        Util.touch(tmpDir + "tractortests/abc/foo/work/baz.txt");
    }

    function cleanup() {
        Util.remove(tmpDir + "tractortests/abc/foo/work/baa.txt");
        Util.remove(tmpDir + "tractortests/abc/foo/work/bar.txt");
        Util.remove(tmpDir + "tractortests/abc/foo/work/baz.txt");
        Util.remove(tmpDir + "tractortests/abc/foo/work/cooked/baa.txt");
        Util.remove(tmpDir + "tractortests/abc/foo/work/cooked/bar.txt");
        Util.remove(tmpDir + "tractortests/abc/foo/work/cooked/baz.txt");
        Util.rmdir(tmpDir + "tractortests/abc/foo/work/cooked");
        Util.rmdir(tmpDir + "tractortests/abc/foo/work");
        Util.rmdir(tmpDir + "tractortests/abc/foo");
        Util.rmdir(tmpDir + "tractortests/abc");
        Util.rmdir(tmpDir + "tractortests");
    }

    Root {
        id: root
        logLevel: Log.Debug

        operations: [
            UpdateOperation {
                id: update
            },
            CookOperation {
                id: cook
                dependencies: update
            },
            TractorOperation {
                id: tractor
                name: "tractor"
                dependencies: update
                target: cook
            }
        ]
        
        fields: [ 
            Field { name: "ROOT"; regexp: tmpDir },
            Field { name: "FOO" },
            Field { name: "FILENAME"; regexp: ".*" }
        ]
        
        Branch {
            id: abc
            name: "abc"
            pattern: "{ROOT}tractortests/abc/"
        }
        
        Branch {
            id: foo
            name: "foo"
            pattern: "{FOO}/"
            root: abc
            
            Branch {
                id: workBranch
                name: "work"
                pattern: "work/"
                root: foo
                
                File {
                    id: workFile
                    name: "workFile"
                    pattern: "{FILENAME}"
                }

                File {
                    id: cookFile
                    name: "cookFile"
                    pattern: "cooked/{FILENAME}"
                    input: workFile
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
        id: tractorSpy
        target: tractor
        signalName: "finished"
    }

    function test_tractorOperationGenerate() {
        var foo;
        var context = {ROOT: tmpDir, FOO: "foo", TITLE: "title of record"};

        tractor.mode = TractorOperation.Submit;
        
        tractor.run(cookFile, context);
        tractorSpy.wait(1000);
        compare(update.status, Operation.Finished);
        compare(tractor.status, Operation.Finished);
        var tractor_data_dir = tractor.tractorJob.asString().match(/TRACTOR_DATA_DIR=([^ ]*)/)[1];
        verify(tractor_data_dir);
    }
    
    function test_tractorOperationExecute() {
        var foo;
        var context = {ROOT: tmpDir, 
                       FOO: "foo", TITLE: "title of record",
                       TRACTOR_DATA_DIR: tmpDir};

        tractor.mode = TractorOperation.Execute;
                
        tractor.run(cookFile, context);
        tractorSpy.wait(1000);
        compare(update.status, Operation.Finished);
        compare(tractor.status, Operation.Finished);
    }
}
