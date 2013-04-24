import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    name: "FileRefTests"
    
    property string tmpDir: "/usr/tmp/pugunittests/filereftests/"

    function init() {
        Util.mkpath(tmpDir + "abc/foo/work");
        Util.touch(tmpDir + "abc/foo/work/bar.txt");
        Util.mkpath(tmpDir + "abc/foo/release/main/v001");
        Util.mkpath(tmpDir + "abc/foo/release/main/v002");
    }

    function cleanup() {
        Util.remove(tmpDir + "abc/foo/work/bar.txt");
        Util.remove(tmpDir + "abc/foo/work/dag.0001.txt");
        Util.remove(tmpDir + "abc/foo/work/dag.0002.txt");
        Util.remove(tmpDir + "abc/foo/work/dag.0003.txt");
        Util.remove(tmpDir + "abc/foo/work/dag.0004.txt");
        Util.remove(tmpDir + "abc/foo/release/alt/v001/bar.txt");
        Util.remove(tmpDir + "abc/foo/release/main/v001/bar.txt");
        Util.remove(tmpDir + "abc/foo/release/main/v002/bar.txt");
        Util.remove(tmpDir + "abc/foo/release/main/v003/bar.txt");
        Util.rmdir(tmpDir + "abc/foo/work");
        Util.rmdir(tmpDir + "abc/foo/release/alt/v001");
        Util.rmdir(tmpDir + "abc/foo/release/main/v001");
        Util.rmdir(tmpDir + "abc/foo/release/main/v002");
        Util.rmdir(tmpDir + "abc/foo/release/main/v003");
        Util.rmdir(tmpDir + "abc/foo/release/main");
        Util.rmdir(tmpDir + "abc/foo/release");
        Util.rmdir(tmpDir + "abc/foo");
        Util.rmdir(tmpDir + "abc");
        Util.rmdir(tmpDir);
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
            ReleaseOperation {
                id: release
                dependencies: cook
            }
        ]
        
        fields: [
            Field { name: "ROOT"; regexp: tmpDir },
            Field { name: "FOO" },
            Field { name: "BAR" },
            Field { name: "DAG" },
            FrameSpecField { name: "FRAME" },
            Field { name: "VARIATION" },
            Field { name: "VERSION"; type: Field.Integer; width: 3 }
        ]
        
        Folder {
            id: abc
            name: "abc"
            pattern: "{ROOT}abc/"
        }
        
        Folder {
            id: foo
            name: "foo"
            pattern: "{FOO}/"
            root: abc
            
            Folder {
                id: releaseBranch
                name: "releaseBranch"
                pattern: "release/{VARIATION}/v{VERSION}/"
                ReleaseOperation.versionField: "VERSION"
                
                File {
                    id: releaseFile
                    name: "releaseFile"
                    pattern: "{BAR}.txt"
                    ReleaseOperation.source: workFile
                    logLevel: Log.Trace
                }
            }
            
            Folder {
                id: workBranch
                name: "workBranch"
                pattern: "work/"
                root: foo
                
                File {
                    id: workFile
                    name: "workFile"
                    pattern: "{BAR}.txt"
                }
                
                FileRef {
                    id: releaseFileRef
                    name: "releaseFileRef"
                    input: releaseFile
                    context: {"VERSION": 3, "VARIATION": "main"}
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

    SignalSpy {
        id: releaseSpy
        target: release
        signalName: "finished"
    }

    function test_fileRef() {
        var context;
        
        releaseSpy.clear();
        context = {ROOT: tmpDir, VARIATION: "main", FOO: "foo", BAR: "bar", DAG: "dag"};
        release.run(releaseFile, context);
        releaseSpy.wait(500);

        compare(workFile.details.length, 1);
        
        compare(releaseBranch.details.length, 2)
        compare(releaseBranch.details[0].element.pattern, tmpDir + "abc/foo/release/main/v001/");
        compare(releaseBranch.details[0].context.VERSION, 1);
        compare(releaseBranch.details[1].element.pattern, tmpDir + "abc/foo/release/main/v002/");
        compare(releaseBranch.details[1].context.VERSION, 2);
        
        compare(releaseFile.details.length, 1);
        compare(releaseFile.details[0].element.pattern, tmpDir + "abc/foo/release/main/v003/bar.txt");
        compare(releaseFile.details[0].context.VERSION, 3);

        releaseSpy.clear();
        context = {ROOT: tmpDir, VARIATION: "alt", FOO: "foo", BAR: "bar", DAG: "dag"};
        release.run(releaseFile, context);
        releaseSpy.wait(500);
        
        compare(workFile.details.length, 1);
        compare(releaseFile.details.length, 1);
        compare(releaseFile.details[0].element.pattern, tmpDir + "abc/foo/release/alt/v001/bar.txt");
        compare(releaseFile.details[0].context.VERSION, 1);
        
        context = {ROOT: tmpDir, FOO: "foo", DAG: "dag"};
        update.run(releaseFileRef, context);
        updateSpy.wait(500);
        
        compare(releaseFile.details.length, 2);
        
        compare(releaseFileRef.details.length, 1);
        compare(releaseFileRef.details[0].element.pattern, tmpDir + "abc/foo/release/main/v003/bar.txt");
        compare(releaseFileRef.details[0].context.VERSION, 3);
    }
    
}