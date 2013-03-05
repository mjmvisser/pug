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
                name: "workBranch"
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
        var context = {ROOT: tmpDir, FOO: "foo", TITLE: "title of record"};

        tractor.mode = TractorOperation.Generate;
        
        tractor.run(cookFile, context);
        tractorSpy.wait(1000);
        compare(tractor.status, Operation.Finished);
        var tractor_data_dir = tractor.tractorJob.asString().match(/TRACTOR_DATA_DIR=([^ ]*)/)[1];
        console.log(tractor.tractorJob.asString());
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
            
            // preceding nodes should all be finished
            for (var j = 0; j <= i; j++) {
                compare(nodes[j].TractorOperation.status, Operation.Finished);
                compare(nodes[j].CookOperation.status, Operation.Finished);
                compare(nodes[j].UpdateOperation.status, Operation.Finished);
                
                if (nodes[i].name == "workFile") {
                    verify(nodes[i].details);
                    compare(nodes[i].details[0].element.path, tmpDir + "tractortests/abc/foo/work/baa.txt");
                    compare(nodes[i].details[1].element.path, tmpDir + "tractortests/abc/foo/work/bar.txt");
                    compare(nodes[i].details[2].element.path, tmpDir + "tractortests/abc/foo/work/baz.txt");
                } else if (nodes[i].name == "cookFile") {
                    verify(nodes[i].details);
                    compare(nodes[i].details[0].element.path, tmpDir + "tractortests/abc/foo/work/cooked/baa.txt");
                    compare(nodes[i].details[1].element.path, tmpDir + "tractortests/abc/foo/work/cooked/bar.txt");
                    compare(nodes[i].details[2].element.path, tmpDir + "tractortests/abc/foo/work/cooked/baz.txt");
                }
            }
            
            compare(update.status, Operation.Finished);
            compare(cook.status, Operation.Finished);
            compare(tractor.status, Operation.Finished);
        }    
        
        verify(Util.exists(tmpDir + "tractortests/abc/foo/work/cooked/baa.txt"));
        verify(Util.exists(tmpDir + "tractortests/abc/foo/work/cooked/bar.txt"));
        verify(Util.exists(tmpDir + "tractortests/abc/foo/work/cooked/baz.txt"));
        
        // cookFile.TractorOperation.logLevel = Log.Debug;
        // workFile.TractorOperation.logLevel = Log.Debug;
        // workBranch.TractorOperation.logLevel = Log.Debug;
        tractor.mode = TractorOperation.Cleanup;

        // cleanup
        tractor.run(cookFile, context);
        tractorSpy.wait(1000);
        compare(tractor.status, Operation.Finished);
    }
}
