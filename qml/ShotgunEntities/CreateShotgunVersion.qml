import QtQuick 2.0
import Pug 1.0
import MokkoTools 1.0
    
ShotgunCreate {
    id: self
    entityType: "Version"

    inputs: [
        Input { name: "sg_project" },
        Input { name: "sg_entity" },
        Input { name: "sg_step" },
        Input { name: "sg_user" },
        Input { name: "frames" },
        Input { name: "sg_publishEvent" },
        Input { name: "__data" }
    ]

    params: [
        Param { name: "code" }
    ]

    returnFields: ["code", "sg_project", "entity", "sg_step", "sg_description", 
                   "sg_path_to_frames", "sg_first_frame", "sg_last_frame", "user"]

    property Node sg_project: null
    property Node sg_entity: null
    property Node sg_step: null
    property Node sg_user: null
    property Branch frames: null
    property Node sg_publishEvent: null

    property string code

    property Node __data: mergeData
    
    ShotgunDataFromEntity {
        id: projectData
        name: "projectData"
        shotgunField: "project"
        entity: sg_project 
    }
    
    ShotgunDataFromEntity {
        id: entityData
        name: "entityData"
        shotgunField: "entity"
        entity: sg_entity
    }

    ShotgunDataFromEntity {
        id: stepData
        name: "stepData"
        shotgunField: "sg_step"
        entity: sg_step
    }
    
    ShotgunDataFromValue {
        id: descriptionData
        name: "descriptionData"
        shotgunField: "description"
        value: "{DESCRIPTION}"
    }

    ShotgunDataFromValue {
        id: codeData
        name: "codeData"
        shotgunField: "code"
        value: format(code, context)
    }

    property var __elementsView: Util.elementsView(frames)    
    
    ShotgunDataFromValue {
        id: firstFrameData
        name: "firstFrameData"
        shotgunField: "sg_first_frame"
        value: __elementsView.element[index].frameStart() 
    }

    ShotgunDataFromValue {
        id: lastFrameData
        name: "lastFrameData"
        shotgunField: "sg_last_frame"
        value: __elementsView.element[index].frameEnd() 
    }
    
    ShotgunDataFromValue {
        id: pathData
        name: "pathData"
        shotgunField: "sg_path_to_frames"
        value: __elementsView.element[index].pattern()
    }

    ShotgunThumbnail {
        id: thumbnail
        name: "thumbnail"
        input: frames
    }

    ShotgunDataFromElement {
        id: imageData
        name: "imageData"
        shotgunField: "image"
        element: thumbnail
    }

    ShotgunThumbnail {
        id: filmstrip
        name: "filmstrip"
        input: frames
        filmstrip: true
    }

    ShotgunDataFromElement {
        id: filmstripData
        name: "filmstripData"
        shotgunField: "filmstrip_image"
        element: filmstrip
    }

    ShotgunDataFromEntity {
        id: createdByData
        name: "createdByData"
        shotgunField: "created_by"
        entity: sg_user
    }
    
    ShotgunDataFromEntity {
        id: updatedByData
        name: "updatedByData"
        shotgunField: "updated_by"
        entity: sg_user
    }

    ShotgunDataFromEntity {
        id: userData
        name: "userData"
        shotgunField: "user"
        entity: sg_user
    }

    ShotgunDataFromEntity {
        id: publishEventData
        name: "publishEventData"
        shotgunField: "sg_publish_event"
        entity: sg_publishEvent
    }
    
    ShotgunQuicktime {
        id: movieMp4
        name: "movieMp4"
        input: frames
        format: "mp4"
        fps: sg_project.sg_frame_rate || 24
    }

    ShotgunDataFromElement {
        id: uploadedMovieData
        name: "uploadedMovieData"
        shotgunField: "sg_uploaded_movie"
        element: movieMp4
    }
    
    MergeShotgunData {
        id: mergeData
        name: "mergeData"
        data: [projectData, entityData, stepData, descriptionData, 
               codeData, firstFrameData, lastFrameData, imageData, 
               filmstripData, createdByData, updatedByData, userData]
    }

    dataValues: {    
        try {
            return [__data.details[0].data];            
        } catch (e) {
            addWarning(e);
        }
    }
}
