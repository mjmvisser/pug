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
        
        Folder {
            id: abc
            name: "abc"
            pattern: "{ROOT}cooktests/abc/"
        }
        
        Folder {
            id: foo
            name: "foo"
            pattern: "{FOO}/"
            root: abc
            
            Folder {
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
                    count: input ? input.count : 0
                    
                    property var input: workFile

                    inputs: Input { name: "input" }

                    signal update(var context)
                    signal updateFinished(int status)
                    
                    onUpdate: {
                        trace("onUpdate(" + context);

                        var inputElementsView = Util.elementsView(input);
                        var elementsView = Util.elementsView(copier);
                        
                        for (index = 0; index < count; index++) {
                            var inputPath = elementsView.elements[index].path();
                            var outputDir = inputElementsView.elements[index].directory() + "temp/";
                            var outputPath = outputDir + "temp_" + inputElementsView.elements[index].baseName() + "." + inputElementsView.elements[index].extension();
    
                            elementsView.elements[index].pattern = outputPath;
                            details[index].context = input.details[index].context;
                            for (var attrname in context) {
                                if (context.hasOwnProperty(attrname)) {
                                    details[index].context[attrname] = context[attrname];
                                }
                            }
                        }
                        
                        updateFinished(Operation.Finished);
                    }

                    signal cook(var context)
                    signal cookFinished(int status)
                    
                    onCook: {
                        trace("onCook(" + context);
                        var inputElementsView = Util.elementsView(input);
                        var elementsView = Util.elementsView(copier);
                        
                        for (index = 0; index < count; index++) {
                            var inputPath = inputElementsView.elements[index].path();
                            var outputDir = elementsView.elements[index].directory();
                            var outputPath = elementsView.elements[index].path();
    
                            if (!Util.exists(outputDir))
                                Util.mkpath(outputDir);
                            Util.copy(inputPath, outputPath);
                        }
                        
                        cookFinished(Operation.Finished);
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
        
        Node {
            id: nodeA
            name: "nodeA"
    
            count: 5
            
            signal cook(var context)
            signal cookFinished(int status)
    
            onCook: {
                for (index = 0; index < count; index++) {
                    details[index] = {"result": name + index,
                                      "context": context};
                }
                detailsChanged();
                cookFinished(Operation.Finished);                              
            }
        }   
        
        TestCompound {
            id: compound
            name: "compound"
            input: nodeA
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
        var context = {ROOT: tmpDir, FOO: "foo"};
        var workPaths = [tmpDir + "cooktests/abc/foo/work/baa.txt",
                         tmpDir + "cooktests/abc/foo/work/bar.txt",
                         tmpDir + "cooktests/abc/foo/work/baz.txt"];
        var cookPaths = [tmpDir + "cooktests/abc/foo/work/cooked/baa.txt",
                         tmpDir + "cooktests/abc/foo/work/cooked/bar.txt",
                         tmpDir + "cooktests/abc/foo/work/cooked/baz.txt"];

        update.run(cookFile, context);
        updateSpy.wait(500);

        var workFileElementsView = Util.elementsView(workFile);
        var cookFileElementsView = Util.elementsView(cookFile);

        compare(update.status, Operation.Finished);
        compare(workFile.UpdateOperation.status, Operation.Finished);
        compare(cookFile.UpdateOperation.status, Operation.Finished);
        compare(workFileElementsView.elements[0].path(), workPaths[0]);
        compare(workFileElementsView.elements[1].path(), workPaths[1]);
        compare(workFileElementsView.elements[2].path(), workPaths[2]);
        compare(cookFile.details.length, 3);

        cook.run(cookFile, context);
        cookSpy.wait(1000);
        compare(cook.status, Operation.Finished);
        compare(copier.count, workFile.count);
        compare(cookFile.count, copier.count);
        compare(workFile.CookOperation.status, Operation.Finished);
        compare(copier.CookOperation.status, Operation.Finished);
        compare(cookFile.CookOperation.status, Operation.Finished);
        compare(workFile.CookOperation.status, Operation.Finished);
        compare(copier.details.length, 3);
        compare(cookFile.details.length, 3);
        compare(cookFileElementsView.elements[0].path(), cookPaths[0]);
        compare(cookFileElementsView.elements[1].path(), cookPaths[1]);
        compare(cookFileElementsView.elements[2].path(), cookPaths[2]);
    }
    
    function test_cookCompound() {
        var context = {};
        cook.run(compound, context);
        cookSpy.wait(500);
        compare(cook.status, Operation.Finished);
        compare(compound.CookOperation.status, Operation.Finished);
        compare(compound.details[0].result, "nodeA0 -> nodeB0 -> nodeC0");
        compare(compound.details[1].result, "nodeA1 -> nodeB1 -> nodeC1");
        compare(compound.details[2].result, "nodeA2 -> nodeB2 -> nodeC2");
        compare(compound.details[3].result, "nodeA3 -> nodeB3 -> nodeC3");
        compare(compound.details[4].result, "nodeA4 -> nodeB4 -> nodeC4");
    }
}
