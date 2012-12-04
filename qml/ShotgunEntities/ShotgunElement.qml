import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

ShotgunEntity {
    name: "Element"

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
        name: "code"
        type: ShotgunField.Pattern
        pattern: code
    }
    
    // doesn't work, sg_source_path is the wrong type (File instead of Entity)
    // ShotgunField {
        // name: "sg_source_path"
        // type: ShotgunField.Link
        // link: sourcePathLink
        // linkType: "Attachment"
    // }
    
    ShotgunField {
        name: "sg_subdir"
        value: __subdir 
    }

    ShotgunField {
        id: startFrameField
        name: "sg_start_frame"
        type: ShotgunField.Number
        value: ShotgunUtils.safeElementAttribute(parent, "firstFrame")                         
    }

    ShotgunField {
        id: endFrameField
        name: "sg_end_frame"
        type: ShotgunField.Number
        value: ShotgunUtils.safeElementAttribute(parent, "lastFrame")                         
    }
    
    ShotgunField {
        name: "sg_delivery"
        type: ShotgunField.Link
        linkType: "Delivery"
        link: delivery      
    }
    
    ShotgunField {
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
        link: project
    }
    
    ShotgunField {
        name: "sg_release"
        type: ShotgunField.Link
        linkType: "PublishEvent"
        link: release
    }
    
    ShotgunField {
        name: "created_by"
        type: ShotgunField.Link
        link: user
        linkType: "HumanUser"
    }

    ShotgunField {
        name: "updated_by"
        type: ShotgunField.Link
        link: user
        linkType: "HumanUser"
    }
}
