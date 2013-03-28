import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

ShotgunEntity {
    shotgunEntity: "Element"

    ShotgunOperation.action: ShotgunOperation.Create

    property ShotgunEntity project
    property ShotgunEntity delivery
    property ShotgunEntity release
    property ShotgunEntity user

    inputs: [
        Input { name: "project" },
        Input { name: "delivery" },
        Input { name: "release" },
        Input { name: "user" }
    ]
    
    property string code
    //property var sourcePathLink
    
    params: Param { name: "code" }
    
    ShotgunField {
        id: codeField
        name: "code"
        shotgunField: "code"
        type: ShotgunField.Pattern
        pattern: code
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
        value: _firstFrame()                       
    }

    ShotgunField {
        id: endFrameField
        name: "sg_end_frame"
        shotgunField: "sg_end_frame"
        type: ShotgunField.Number
        value: _lastFrame()                         
    }
    
    ShotgunField {
        name: "sg_delivery"
        shotgunField: "sg_delivery"
        type: ShotgunField.Link
        link: delivery      
    }
    
    ShotgunField {
        name: "project"
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        name: "sg_release"
        shotgunField: "sg_release"
        type: ShotgunField.Link
        link: release
    }
    
    ShotgunField {
        name: "created_by"
        shotgunField: "created_by"
        type: ShotgunField.Link
        link: user
    }

    ShotgunField {
        name: "updated_by"
        shotgunField: "updated_by"
        type: ShotgunField.Link
        link: user
    }
}
