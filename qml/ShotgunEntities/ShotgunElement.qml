import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

    
ShotgunEntity {
    id: elementEntity
    name: "elementEntity"
    shotgunEntity: "Element"
    shotgunFields: [
        codeField,
        startFrameField,
        endFrameField,
        deliveryField,
        projectField,
        releaseField,
        createdByField,
        updatedByField
    ]

    property Node project
    property Node delivery
    property Node release
    property Node user

    inputs: [
        Input { name: "project" },
        Input { name: "delivery" },
        Input { name: "release" },
        Input { name: "user" }
    ]
    
    count: parent.count

    property string code
    //property var sourcePathLink
    
    params: [
        Param { name: "code" }
    ]

    ShotgunField {
        id: codeField
        name: "code"
        required: true
        shotgunField: "code"
        type: ShotgunField.String
        pattern: code
        source: elementEntity.parent
    }
    
    // doesn't work, sg_source_path is the wrong type (File instead of Entity)
    // ShotgunField {
        // shotgunField: "sg_source_path"
        // type: ShotgunField.Link
        // link: sourcePathLink
    // }
    
    function _firstFrame(index) {
        var elementsView = Util.elementsView(parent);

        try {        
            return elementsView.elements[index].firstFrame();
        } catch (e) {
            return null;
        } finally {
            elementsView.destroy();
        }
    }

    function _lastFrame(index) {
        var elementsView = Util.elementsView(parent);
        
        try {        
            return elementsView.elements[index].lastFrame();
        } catch (e) {
            return null;
        } finally {
            elementsView.destroy();
        }
    }

    ShotgunField {
        id: startFrameField
        name: "sg_start_frame"
        shotgunField: "sg_start_frame"
        type: ShotgunField.Number
        value: _firstFrame(index)                       
        source: elementEntity.parent
    }

    ShotgunField {
        id: endFrameField
        name: "sg_end_frame"
        shotgunField: "sg_end_frame"
        type: ShotgunField.Number
        value: _lastFrame(index)
        source: elementEntity.parent
    }
    
    ShotgunField {
        id: deliveryField
        name: "sg_delivery"
        shotgunField: "sg_delivery"
        type: ShotgunField.Link
        link: delivery      
    }
    
    ShotgunField {
        id: projectField
        name: "project"
        shotgunField: "project"
        required: true
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        id: releaseField
        name: "sg_release"
        shotgunField: "sg_release"
        type: ShotgunField.Link
        link: release
    }
    
    ShotgunField {
        id: createdByField
        name: "created_by"
        shotgunField: "created_by"
        type: ShotgunField.Link
        link: user
    }

    ShotgunField {
        id: updatedByField
        name: "updated_by"
        shotgunField: "updated_by"
        type: ShotgunField.Link
        link: user
    }
}
