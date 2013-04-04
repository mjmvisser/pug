import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    name: "ShotgunFieldTests"
    
    property string testImage: Qt.resolvedUrl("SMPTE_Color_Bars_16x9.png").replace("file://", "")

    function init() {
        Shotgun.baseUrl = "https://mokko.shotgunstudio.com";
        Shotgun.apiKey = "ef0623879e54c9f4f2d80f9502a7adea09bfcf8f";
        Shotgun.scriptName = "test";
        
        Util.mkpath(tmpDir + "shotgunfieldtests/projects/888_test");
        Util.copy(testImage, tmpDir + "shotgunfieldtests/projects/888_test/string_999.png");
    }
    
    function cleanup() {
        Util.remove(tmpDir + "shotgunfieldtests/projects/888_test/string_999.png");
        Util.rmdir(tmpDir + "shotgunfieldtests/projects/888_test");
        Util.rmdir(tmpDir + "shotgunfieldtests/projects");
        Util.rmdir(tmpDir + "shotgunfieldtests");
        updateSpy.clear();
        releaseSpy.clear();
    }
    
    Root {
        id: root

        operations: [
            UpdateOperation {
                id: update
            },
            ReleaseOperation {
                id: release
                dependencies: update
            }
        ]
    
        fields: [
            Field { name: "STRING" },
            Field { name: "NUMBER"; type: Field.Integer },
            Field { name: "PROJECT" }
        ]

        Folder {
            id: project
            name: "project"
            pattern: tmpDir + "shotgunfieldtests/projects/{PROJECT}/"

            ShotgunField {
                id: sg_projectName
                name: "sg_projectName"
                shotgunField: "name"
                required: true
                field: "PROJECT"
                source: project
            }
            
            ShotgunEntity {
                id: sg_project
                name: "sg_project"
                shotgunEntity: "Project"
                shotgunFields: sg_projectName
            }
        }
            
        File {
            id: file
            name: "file"
            root: project
            pattern: "{STRING}_{NUMBER}.png"

            ShotgunEntity {
                id: sg_testEntity
                name: "sg_testEntity"
                shotgunEntity: "PublishEvent"
                action: ShotgunEntity.None
                shotgunFields: [
                    sg_patternField, 
                    sg_imageField, 
                    sg_linkField, 
                    sg_pathField, 
                    sg_numberField, 
                    sg_stringField, 
                    sg_stringValueField
                ]
                
                ShotgunField {
                    id: sg_patternField
                    name: "sg_patternField"
                    shotgunField: "code"
                    required: true
                    type: ShotgunField.String
                    source: file
                    pattern: "{STRING}_{NUMBER}"
                }
                
                ShotgunField {
                    id: sg_imageField
                    name: "sg_imageField"
                    shotgunField: "image"
                    type: ShotgunField.Path
                    source: file
                }
                
                ShotgunField {
                    id: sg_linkField
                    name: "sg_linkField"
                    shotgunField: "project"
                    type: ShotgunField.Link
                    link: sg_project
                }
            
            ShotgunField {
                id: sg_pathField
                name: "sg_pathField"
                shotgunField: "sg_path"
                type: ShotgunField.Path
                source: file
            }
            
            ShotgunField {
                id: sg_numberField
                name: "sg_numberField"
                shotgunField: "sg_version"
                type: ShotgunField.Number
                field: "NUMBER"
                source: file
            }
            
            ShotgunField {
                id: sg_stringField
                name: "sg_stringField"
                shotgunField: "sg_variation"
                type: ShotgunField.String
                field: "STRING"
                source: file
            }
            
            ShotgunField {
                id: sg_stringValueField
                name: "sg_stringValueField"
                shotgunField: "sg_layer"
                type: ShotgunField.String
                value: "foo"
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
        id: releaseSpy
        target: release
        signalName: "finished"
    }
    
    function test_updateField_data() {
        return [
            {tag: "patternField", sg_field: sg_patternField, values: ["string_999"], dependencies: [file]},
            {tag: "linkField", sg_field: sg_linkField, values: [{type: "Project", id: 104}], dependencies: [sg_project, project]},
            {tag: "pathField", sg_field: sg_pathField, values: [tmpDir + "shotgunfieldtests/projects/888_test/string_999.png"], dependencies: [file, project]},
            {tag: "numberWang", sg_field: sg_numberField, values: [999], dependencies: [file]},
            {tag: "stringField", sg_field: sg_stringField, values: ["string"], dependencies: [file]},
            {tag: "stringValueField", sg_field: sg_stringValueField, values: ["foo"], dependencies: [file]}
        ];
    }
    
    function test_releaseField_data() {
        return test_updateField_data();
    }
    
    function test_updateField(data) {
        sg_testEntity.action = ShotgunEntity.Find;
        
        update.run(data.sg_field, {});
        updateSpy.wait(5000);

        compare(data.sg_field.UpdateOperation.status, Operation.Finished);
        for (var i = 0; i < data.dependencies.length; i++) {
            compare(data.dependencies[i].UpdateOperation.status, Operation.Finished);
        }

        for (var i = 0; i < data.sg_field.details.length; i++) {
            compare(data.sg_field.details[i].value, data.values[i]);
        }
    }
    
    function test_releaseField(data) {
        sg_testEntity.action = ShotgunEntity.Create;
        
        release.run(data.sg_field, {});
        releaseSpy.wait(5000);

        compare(data.sg_field.ReleaseOperation.status, Operation.Finished);
        for (var i = 0; i < data.dependencies.length; i++) {
            compare(data.dependencies[i].ReleaseOperation.status, Operation.Finished);
        }

        for (var i = 0; i < data.sg_field.details.length; i++) {
            compare(data.sg_field.details[i].value, data.values[i]);
        }
    }
}
