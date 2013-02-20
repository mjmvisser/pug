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
        Util.remove(tmpDir + "tractortests/abc/foo/work/temp/temp_baa.txt");
        Util.remove(tmpDir + "tractortests/abc/foo/work/temp/temp_bar.txt");
        Util.remove(tmpDir + "tractortests/abc/foo/work/temp/temp_baz.txt");
        Util.remove(tmpDir + "tractortests/abc/foo/work/cooked/baa.txt");
        Util.remove(tmpDir + "tractortests/abc/foo/work/cooked/bar.txt");
        Util.remove(tmpDir + "tractortests/abc/foo/work/cooked/baz.txt");
        Util.rmdir(tmpDir + "tractortests/abc/foo/work/temp");
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

                Node {
                    id: copier
                    name: "copier"
                    count: input ? input.details.length : 0
                    
                    property var input: workFile

                    inputs: Input { name: "input" }

                    signal cookAtIndex(int index, var context)
                    signal cookedAtIndex(int index, int status)
                    
                    onCookAtIndex: {
                        debug("onCookAtIndex");
                        
                        debug("input is " + copier.input);
                        debug("index is " + index);
                        debug("input.details[index].element is " + copier.input.details[index].element);
                        debug("input.details[index].element.path is " + copier.input.details[index].element.path);
                        debug("input.details[index].context " + JSON.stringify(copier.input.details[index].context));
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
                        newElement.pattern = outputPath;
                        
                        details[index] = {"element": newElement, "context": copier.input.details[index].context};
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
        id: tractorSpy
        target: tractor
        signalName: "finished"
    }

    function test_tractorOperation() {
        var foo;
        var context = {ROOT: tmpDir, FOO: "foo", TITLE: "title of record"};

        tractor.run(cookFile, context);
        tractorSpy.wait(1000);
        compare(update.status, Operation.Finished);
        compare(tractor.status, Operation.Finished);
        console.log(tractor.tractorJob.asString());
    }
}
