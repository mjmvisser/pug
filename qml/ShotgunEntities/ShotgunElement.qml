import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

ShotgunEntity {
    shotgunEntity: "Element"

    property var project
    property var delivery
    property var release
    property var user

    property string code
    //property var sourcePathLink
    
    inputs: [
        Input { name: "project" },
        Input { name: "delivery" },
        Input { name: "release" },
        Input { name: "user" }
    ]
    
    params: Param { name: "code" }
    
    property var __subdir: {
        var parentDir = ShotgunUtils.safeElementAttribute(parent, "directory");
        var grandparentDir =  ShotgunUtils.safeElementAttribute(parent.node(".."), "directory");
        if (parentDir && grandparentDir) {
            return parentDir.replace(grandparentDir, "");
        } else {
            return null;
        }
    }

    ShotgunField {
        id: codeField
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
    
    ShotgunField {
        shotgunField: "sg_subdir"
        value: __subdir 
    }

    ShotgunField {
        id: startFrameField
        shotgunField: "sg_start_frame"
        type: ShotgunField.Number
        value: parent.details[index].element.firstFrame                         
    }

    ShotgunField {
        id: endFrameField
        shotgunField: "sg_end_frame"
        type: ShotgunField.Number
        value: parent.details[index].element.lastFrame                         
    }
    
    ShotgunField {
        shotgunField: "sg_delivery"
        type: ShotgunField.Link
        link: delivery      
    }
    
    ShotgunField {
        shotgunField: "project"
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        shotgunField: "sg_release"
        type: ShotgunField.Link
        link: release
    }
    
    ShotgunField {
        shotgunField: "created_by"
        type: ShotgunField.Link
        link: user
    }

    ShotgunField {
        shotgunField: "updated_by"
        type: ShotgunField.Link
        link: user
    }
}
