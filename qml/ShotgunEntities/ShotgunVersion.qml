import QtQuick 2.0
import Pug 1.0
import MokkoTools 1.0

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
        updatedByField,
        userField,
        publishEventField,
        uploadedMovieField
        //uploadedMovieMp4Field,
        //uploadedMovieWebmField     
    ]
    
    property Node project: null
    property Node link: null
    property Node step: null
    property Node user: null
    property Node frames: null
    property Node publishEvent: null

    inputs: [
        Input { name: "project" },
        Input { name: "link" },
        Input { name: "step" },
        Input { name: "user" },
        Input { name: "frames" },
        Input { name: "publishEvent" }
    ]

    output: true

    count: parent.count

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
        source: frames
    }

    function _firstFrame(index) {
        var elementsView = Util.elementsView(frames);

        try {        
            return elementsView.elements[index].frameStart();
        } catch (e) {
            return null;
        }
    }

    function _lastFrame(index) {
        var elementsView = Util.elementsView(frames);
        
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
        value: frames ? _firstFrame(index) : 0                         
        source: frames
    }

    ShotgunField {
        id: lastFrameField
        name: "lastFrameField"
        shotgunField: "sg_last_frame"
        type: ShotgunField.Number
        value: frames ? _lastFrame(index) : 0                         
        source: frames
    }
    
    ShotgunThumbnail {
        id: thumbnail
        name: "thumbnail"
        input: frames
    }

    ShotgunField {
        id: imageField
        name: "imageField"
        shotgunField: "image"
        type: ShotgunField.Path
        source: thumbnail
    }

    ShotgunThumbnail {
        id: filmstrip
        name: "filmstrip"
        input: frames
        filmstrip: true
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
    
    ShotgunField {
        id: userField
        name: "userField"
        shotgunField: "user"
        type: ShotgunField.Link
        link: user
    }
    
    ShotgunField {
        id: publishEventField
        name: "publishEventField"
        shotgunField: "sg_publish_event"
        type: ShotgunField.Link
        link: publishEvent
    }
    
    ShotgunQuicktime {
        id: movieMp4
        name: "movieMp4"
        input: frames
        format: "mp4"
        fps: (((project || {}).details[0] || {}).entity || {}).sg_frame_rate || 24
    }

    ShotgunField {
        id: uploadedMovieField
        name: "uploadedMovieField"
        shotgunField: "sg_uploaded_movie"
        type: ShotgunField.Path
        source: movieMp4
    }
    
    // ShotgunField {
        // id: uploadedMovieMp4Field
        // name: "uploadedMovieMp4Field"
        // shotgunField: "sg_uploaded_movie_mp4"
        // type: ShotgunField.Path
        // source: movieMp4
    // }
// 
    // ShotgunQuicktime {
        // id: movieWebm
        // name: "movieWebm"
        // input: frames
        // format: "webm"
        // fps: (((project || {}).details[0] || {}).entity || {}).sg_frame_rate || 24
    // }
// 
    // ShotgunField {
        // id: uploadedMovieWebmField
        // name: "uploadedMovieWebmField"
        // shotgunField: "sg_uploaded_movie_webm"
        // type: ShotgunField.Path
        // source: movieWebm
    // }
}
