import QtQuick 2.0
import Pug 1.0

import "js/shotgunutils.js" as ShotgunUtils
    
ShotgunEntity {
    id: versionEntity
    name: "versionEntity"
    shotgunEntity: "Version"
    shotgunFields: [
        codeField,
        projectField,
        entityField,
        stepField,
        descriptionField,
        pathField,
        firstFrameField,
        lastFrameField,
        imageField,
        filmstripField,
        createdByField,
        updatedByField         
    ]
    
    property Node project: null
    property Node link: null
    property Node step: null
    property Node thumbnail: null
    property Node filmstrip: null
    property Node user: null

    inputs: [
        Input { name: "project" },
        Input { name: "link" },
        Input { name: "step" },
        Input { name: "thumbnail" },
        Input { name: "filmstrip" },
        Input { name: "user" }
    ]

    output: true

    property string code

    params: [
        Param { name: "code" }
    ]
    
    ShotgunField {
        id: codeField
        name: "codeField"
        shotgunField: "code"
        required: true
        type: ShotgunField.String
        pattern: code
        source: versionEntity.parent
    }

    ShotgunField {
        id: projectField
        name: "projectField"
        shotgunField: "project"
        required: true
        type: ShotgunField.Link
        link: project
    }
    
    ShotgunField {
        id: entityField
        name: "entityField"
        shotgunField: "entity"
        type: ShotgunField.Link
        link: versionEntity.link
    }

    ShotgunField {
        id: stepField
        name: "stepField"
        shotgunField: "sg_step"
        type: ShotgunField.Link
        link: step
    }

    ShotgunField {
        id: descriptionField
        name: "descriptionField"
        shotgunField: "description"
    }
    
    ShotgunField {
        id: pathField
        name: "pathField"
        shotgunField: "sg_path_to_frames"
        type: ShotgunField.Path
        source: versionEntity.parent
    }

    function _firstFrame(index) {
        var elementsView = Util.elementsView(parent);

        try {        
            return elementsView.elements[index].frameStart();
        } catch (e) {
            return null;
        }
    }

    function _lastFrame(index) {
        var elementsView = Util.elementsView(parent);
        
        try {        
            return elementsView.elements[index].frameEnd();
        } catch (e) {
            return null;
        }
    }

    ShotgunField {
        id: firstFrameField
        name: "firstFrameField"
        shotgunField: "sg_first_frame"
        type: ShotgunField.Number
        value: _firstFrame(index)                         
        source: versionEntity.parent
    }

    ShotgunField {
        id: lastFrameField
        name: "lastFrameField"
        shotgunField: "sg_last_frame"
        type: ShotgunField.Number
        value: _lastFrame(index)                         
        source: versionEntity.parent
    }
    
    ShotgunField {
        id: imageField
        name: "imageField"
        shotgunField: "image"
        type: ShotgunField.Path
        source: thumbnail
    }

    ShotgunField {
        id: filmstripField
        name: "filmstripField"
        shotgunField: "filmstrip_image"
        type: ShotgunField.Path
        source: filmstrip
    }

    ShotgunField {
        id: createdByField
        name: "createdByField"
        shotgunField: "created_by"
        type: ShotgunField.Link
        link: user
    }

    ShotgunField {
        id: updatedByField
        name: "updatedByField"
        shotgunField: "updated_by"
        type: ShotgunField.Link
        link: user
    }
}
