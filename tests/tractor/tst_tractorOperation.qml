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
                target: cook
            }
        ]
        
        fields: [ 
            Field { name: "ROOT"; regexp: tmpDir },
            Field { name: "FOO" },
            Field { name: "FILENAME"; regexp: ".*" }
        ]
        
        Branch {
            TractorOperation.logLevel: Log.Debug
            id: abc
            name: "abc"
            pattern: "{ROOT}tractortests/abc/"
        }
        
        Branch {
            TractorOperation.logLevel: Log.Debug
            id: foo
            name: "foo"
            pattern: "{FOO}/"
            root: abc
            
            Branch {
                TractorOperation.logLevel: Log.Debug
                id: workBranch
                name: "work"
                pattern: "work/"
                root: foo
                
                File {
                    TractorOperation.logLevel: Log.Debug
                    id: workFile
                    name: "workFile"
                    pattern: "{FILENAME}"
                }

                File {
                    TractorOperation.logLevel: Log.Debug
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
        var context = {ROOT: tmpDir, FOO: "foo", TITLE: "title of record"};

        tractor.mode = TractorOperation.Submit;
        
        tractor.run(cookFile, context);
        tractorSpy.wait(1000);
        compare(tractor.status, Operation.Finished);
        var tractor_data_dir = tractor.tractorJob.asString().match(/TRACTOR_DATA_DIR=([^ ]*)/)[1];
        verify(tractor_data_dir);
    }
    
    function test_tractorOperationExecute() {
        var context = {ROOT: tmpDir, 
                       FOO: "foo", TITLE: "title of record",
                       TRACTOR_DATA_DIR: tmpDir};

        tractor.mode = TractorOperation.Execute;

        var nodes = [abc, foo, workBranch, workFile, cookFile];
        for (var i = 0; i < nodes.length; i++) {
            tractor.run(nodes[i], context);
            tractorSpy.wait(1000);
            compare(update.status, Operation.Finished);
            compare(cook.status, Operation.Finished);
            compare(tractor.status, Operation.Finished);
        }    
        
        verify(Util.exists(tmpDir + "tractortests/abc/foo/work/cooked/baa.txt"));
        verify(Util.exists(tmpDir + "tractortests/abc/foo/work/cooked/bar.txt"));
        verify(Util.exists(tmpDir + "tractortests/abc/foo/work/cooked/baz.txt"));
        
        tractor.mode = TractorOperation.Cleanup;

        // cleanup
        for (var i = 0; i < nodes.length; i++) {
            tractor.run(nodes[i], context);
            tractorSpy.wait(1000);
            compare(tractor.status, Operation.Finished);
        }        
    }
}
