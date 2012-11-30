import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils

ShotgunEntity {
    name: "Element"

    property alias project: projectField.link
    property alias delivery: deliveryField.link
    property alias release: releaseField.link
    property alias code: codeField.pattern
    property var user
    property var sourcePathLink

    property var subdir: {
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
        value: subdir 
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
        id: deliveryField
        name: "sg_delivery"
        type: ShotgunField.Link
        linkType: "Delivery"        
    }
    
    ShotgunField {
        id: projectField
        name: "project"
        type: ShotgunField.Link
        linkType: "Project"
    }
    
    ShotgunField {
        id: releaseField
        name: "sg_release"
        type: ShotgunField.Link
        linkType: "PublishEvent"
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
