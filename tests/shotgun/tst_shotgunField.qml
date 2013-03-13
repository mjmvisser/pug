import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    name: "ShotgunOperationTests"
    
    property string testImage: Qt.resolvedUrl("SMPTE_Color_Bars_16x9.png").replace("file://", "")
    
    Root {
        id: root

        Shotgun {
            id: shotgun
            baseUrl: "https://mokko.shotgunstudio.com"
            apiKey: "ef0623879e54c9f4f2d80f9502a7adea09bfcf8f"
            scriptName: "test"
        }
        
        operations: [
            UpdateOperation {
                id: update
            },
            ShotgunOperation {
                id: shotgunPull
                name: "shotgunPull"
                mode: ShotgunOperation.Pull
                shotgun: shotgun
                dependencies: update
            },
            ShotgunOperation {
                id: shotgunPush
                name: "shotgunPush"
                mode: ShotgunOperation.Push
                shotgun: shotgun
                dependencies: shotgunPull
            }
        ]
    
        fields: [
            Field { name: "FILENAME" },
            Field { name: "PROJECT" }
        ]

        Folder {
            id: project
            name: "project"
            pattern: "/prod/projects/{PROJECT}"
            
            ShotgunEntity {
                id: sg_project
                name: "sg_project"
                shotgunEntity: "Project"
                
                ShotgunField {
                    id: sg_projectName
                    name: "sg_projectName"
                    shotgunField: "name"
                    field: "PROJECT"
                }
            }
        }

        File {
            id: file
            name: "file"
            pattern: "{FILENAME}"
            
            ShotgunEntity {
                shotgunEntity: "PublishEvent"
                ShotgunOperation.action: ShotgunOperation.Create
                id: sg_release
                
                ShotgunField {
                    shotgunField: "code"
                    type: ShotgunField.Pattern
                    file: file
                    pattern: "{FILENAME}_upload"
                }
                
                ShotgunField {
                    shotgunField: "image"
                    type: ShotgunField.Path
                    file: file
                }
                
                ShotgunField {
                    shotgunField: "project"
                    type: ShotgunField.Link
                    link: sg_project
                }
            }
        }
    }
    
    SignalSpy {
        id: pullSpy
        target: shotgunPull
        signalName: "finished"
    }

    SignalSpy {
        id: pushSpy
        target: shotgunPush
        signalName: "finished"
    }
    
    function test_pullFields() {
        var context = {PROJECT: "888_test",
                       FILENAME: testImage};
        shotgunPull.run(project, context);
        pullSpy.wait(5000);

        compare(shotgunPull.status, Operation.Finished);
        
        compare(project.count, 1);
        compare(sg_project.count, 1);
        compare(sg_projectName.count, 1);
        compare(sg_project.details[0].entity.type, "Project");
        compare(sg_project.details[0].entity.id, 104);
        compare(sg_project.details[0].entity.name, "888_test");
    }
    
    function test_pushFields() {
        var context = {PROJECT: "888_test",
                       FILENAME: testImage};
        shotgunPush.run(file, context);
        pushSpy.wait(5000);
        
        compare(shotgunPush.status, Operation.Finished);
        
        compare(project.count, 1);
        compare(sg_project.count, 1);
        compare(sg_projectName.count, 1);
        compare(sg_project.details[0].entity.type, "Project");
        compare(sg_project.details[0].entity.id, 104);
        compare(sg_project.details[0].entity.name, "888_test");
        
        compare(file.count, 1);
        compare(sg_release.count, 1);
        compare(sg_release.details[0].entity.type, "PublishEvent");
    }
}
