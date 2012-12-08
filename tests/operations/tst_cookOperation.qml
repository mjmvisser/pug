import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "CookOperationTests"

    function init() {
        Util.mkpath(tmpDir + "cooktests/abc/foo/work");
        Util.touch(tmpDir + "cooktests/abc/foo/work/baa.txt");
        Util.touch(tmpDir + "cooktests/abc/foo/work/bar.txt");
        Util.touch(tmpDir + "cooktests/abc/foo/work/baz.txt");
    }

    function cleanup() {
        Util.remove(tmpDir + "cooktests/abc/foo/work/baa.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/bar.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/baz.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/temp/temp_baa.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/temp/temp_bar.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/temp/temp_baz.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/cooked/baa.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/cooked/bar.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/cooked/baz.txt");
        Util.rmdir(tmpDir + "cooktests/abc/foo/work/temp");
        Util.rmdir(tmpDir + "cooktests/abc/foo/work/cooked");
        Util.rmdir(tmpDir + "cooktests/abc/foo/work");
        Util.rmdir(tmpDir + "cooktests/abc/foo");
        Util.rmdir(tmpDir + "cooktests/abc");
        Util.rmdir(tmpDir + "cooktests");
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
            pattern: "{ROOT}cooktests/abc/"
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

                Node {
                    logLevel: Log.Debug
                    id: copier
                    name: "copier"
                    count: input ? input.details.length : 0
                    
                    property var input: workFile

                    inputs: Input { name: "input" }

                    signal cookAtIndex(int index, var env)
                    signal cookedAtIndex(int index, int status)
                    
                    onCookAtIndex: {
                        debug("onCookAtIndex");
                        
                        debug("input is " + copier.input);
                        debug("index is " + index);
                        debug("input.details[index].element is " + copier.input.details[index].element);
                        debug("input.details[index].element.path is " + copier.input.details[index].element.path);
                        debug("input.details[index].env " + JSON.stringify(copier.input.details[index].env));
                        debug("input.count is " + input.count);
                        debug("count is " + count);
                        var inputPath = copier.input.details[index].element.path;
                        var outputDir = copier.input.details[index].element.directory + "temp/";
                        var outputPath = outputDir + "temp_" + copier.input.details[index].element.baseName + "." + copier.input.details[index].element.extension;

                        debug("--- creating " + outputPath + " from " + inputPath);
                        if (!Util.exists(outputDir))
                            Util.mkpath(outputDir);
                        Util.copy(inputPath, outputPath);
                        
                        var newElement = Util.newElement();
                        
                        details[index] = {"element": newElement, "env": copier.input.details[index].env};
                        detailsChanged();
                        cookedAtIndex(index, Operation.Finished);
                    }
                }
                
                File {
                    id: cookFile
                    name: "cookFile"
                    pattern: "cooked/{FILENAME}"
                    input: copier
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

    function test_cookFile() {
        var env = {ROOT: tmpDir, FOO: "foo"};
        var workPaths = [tmpDir + "cooktests/abc/foo/work/baa.txt",
                         tmpDir + "cooktests/abc/foo/work/bar.txt",
                         tmpDir + "cooktests/abc/foo/work/baz.txt"];
        var cookPaths = [tmpDir + "cooktests/abc/foo/work/cooked/baa.txt",
                         tmpDir + "cooktests/abc/foo/work/cooked/bar.txt",
                         tmpDir + "cooktests/abc/foo/work/cooked/baz.txt"];

        update.run(cookFile, env);
        updateSpy.wait(500);
        compare(update.status, Operation.Finished);
        compare(workFile.UpdateOperation.status, Operation.Finished);
        compare(cookFile.UpdateOperation.status, Operation.Finished);
        compare(workFile.details[0].element.path, workPaths[0]);
        compare(workFile.details[1].element.path, workPaths[1]);
        compare(workFile.details[2].element.path, workPaths[2]);
        compare(cookFile.details.length, 0);
        
        cook.run(cookFile, env);
        cookSpy.wait(1000);
        compare(cook.status, Operation.Finished);
        console.log("workfile has " + workFile.details.length + " details and a count of " + workFile.count);
        console.log("copier has " + copier.details.length + " details and a count of " + copier.count);
        console.log("cookFile has " + cookFile.details.length + " details and a count of " + cookFile.count);
        compare(copier.count, workFile.count);
        compare(cookFile.count, copier.count);
        compare(workFile.CookOperation.status, Operation.Finished);
        compare(copier.CookOperation.status, Operation.Finished);
        compare(cookFile.CookOperation.status, Operation.Finished);
        compare(workFile.CookOperation.status, Operation.Finished);
        compare(cookFile.details.length, 3);
        compare(cookFile.details.length, 3);
        compare(cookFile.details[0].element.path, cookPaths[0]);
        compare(cookFile.details[1].element.path, cookPaths[1]);
        compare(cookFile.details[2].element.path, cookPaths[2]);
    }
}
