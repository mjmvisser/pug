import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    id: self
    name: "ShotgunBatchTests"

    function init() {
        //Shotgun.logLevel = Log.Warning;
        Shotgun.baseUrl = "https://mokko.shotgunstudio.com";
        Shotgun.apiKey = "ef0623879e54c9f4f2d80f9502a7adea09bfcf8f";
        Shotgun.scriptName = "test";
    }
        
    Root {
        //logLevel: Log.Info
        id: root

        operations: [
            UpdateOperation {
                id: update
                name: "update"
            },
            ReleaseOperation {
                id: release
                name: "release"
                dependencies: update
            }
        ]
        
        ShotgunFind {
            id: findProject
            name: "findProject"
            entityType: "Project"
            filters: [["name", "is", "888_test"]]
            fields: ["name", "sg_description", "sg_full_name"]
        }
        
        ShotgunFind {
            id: findDelivery
            name: "findDelivery"

            property var project: findProject
            inputs: Input { name: "project" }

            entityType: "Delivery"
            
            filters: [["project", "is", project.details[0].entity], ["title", "is", "test"]]
        }

        ShotgunFind {
            id: findPublishEvent
            name: "findPublishEvent"
            
            property var project: findProject
            property var delivery: findDelivery

            inputs: [
                Input { name: "project" },
                Input { name: "delivery" }
            ]

            entityType: "PublishEvent"
            filters: [["project", "is", project.details[0].entity], ["publish_event_links", "is", delivery.details[0].entity]]
            fields: ["code"]
            limit: 1
        }
        
        ShotgunBatch {
            id: batchPublishEvent

            property var project: findProject
            property var publishEvent: findPublishEvent

            inputs: [
                Input { name: "project" },
                Input { name: "publishEvent" }
            ]

            requests: [{"request_type": "update", "entity_type": "PublishEvent", "entity_id": publishEvent.details[0].entity.id, "data": {"code": publishEvent.details[0].entity.code + "A"}},
                       {"request_type": "create", "entity_type": "PublishEvent", "data": {"code": "test", "project": project.details[0].entity}}]            
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
    
    function test_shotgunFind() {
        // make sure the finds are working
        update.run(batchPublishEvent, {});
        updateSpy.wait(5000);
        
        verify(batchPublishEvent.data);
        compare(batchPublishEvent.requests[0].data.code, findPublishEvent.details[0].entity.code + "A");
        compare(batchPublishEvent.requests[1].data.code, "test");
    }
    
    function test_shotgunBatch() {
        release.run(batchPublishEvent, {});
        releaseSpy.wait(5000);
        
        compare(batchPublishEvent.details.length, 2);
        compare(batchPublishEvent.details[0].context, {});
        compare(batchPublishEvent.details[0].entity.type, "PublishEvent");
        compare(batchPublishEvent.details[0].entity.code, findPublishEvent.details[0].entity.code + "A");
        compare(batchPublishEvent.details[1].context, {});
        compare(batchPublishEvent.details[1].entity.type, "PublishEvent");
        compare(batchPublishEvent.details[1].entity.code, "test");
    }
}
