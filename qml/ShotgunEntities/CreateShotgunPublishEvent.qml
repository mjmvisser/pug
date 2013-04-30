import Pug 1.0
import MokkoTools 1.0

ShotgunCreate {
    id: self
    entityType: "PublishEvent"

    inputs: [
        Input { name: "sg_project" },
        Input { name: "sg_entity" },
        Input { name: "sg_step" },
        Input { name: "sg_user" },
        Input { name: "sg_dependencies" },
        Input { name: "frames" },
        Input { name: "__data" }
        
    ]

    params: [
        Param { name: "code" },
        Param { name: "category" }
    ]

    returnFields: ["code", "sg_project", "entity", "sg_step", "sg_description"]

    property Node sg_project: null
    property Node sg_entity: null
    property Node sg_step: null
    property Node sg_user: null 
    property list<Node> sg_dependencies
    property Node frames: null

    property string code
    property string category

    property Node __data: mergeData

    ShotgunDataFromEntity {
        id: projectData
        name: "projectData"
        shotgunField: "project"
        entity: sg_project
    }

    ShotgunDataFromValue {
        id: codeData
        name: "codeData"
        shotgunField: "code"
        value: code
    }

    ShotgunDataFromValue {
        id: categoryData
        name: "categoryData"
        shotgunField: "sg_category"
        value: category
    }

    ShotgunDataFromValue {
        id: resolutionData
        name: "resolutionData"
        shotgunField: "sg_res"
        value: "{RESOLUTION}"
    }

    ShotgunDataFromValue {
        id: versionData
        name: "versionData"
        shotgunField: "sg_version"
        value: "{VERSION}"
    }
    
    ShotgunDataFromValue {
        id: variationData
        name: "variationData"
        shotgunField: "sg_variation"
        value: "{VARIATION}"
    }
    
    ShotgunDataFromElement {
        id: pathData
        name: "pathData"
        shotgunField: "sg_path"
        element: frames
    }

    ShotgunDataFromEntity {
        id: entityData
        name: "entityData"
        shotgunField: "sg_link"
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
    
    ShotgunThumbnail {
        id: thumbnail
        name: "thumbnail"
        input: frames
    }

    ShotgunDataFromElement {
        id: imageData
        name: "imageField"
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
        name: "filmstripField"
        shotgunField: "filmstrip_image"
        element: filmstrip
    }

    ShotgunDataFromValue {
        id: dependenciesData
        name: "dependenciesData"
        shotgunField: "sg_dependencies"
        value: sg_dependencies
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

    MergeShotgunData {
        id: mergeData
        name: "mergeData"
        data: [projectData, codeData, categoryData, resolutionData, 
               versionData, variationData, pathData, entityData, stepData, 
               descriptionData, imageData, filmstripData, createdByData, updatedByData]
    }

    dataValues: {    
        try {
            return [__projectFilter.details[0].data];            
        } catch (e) {
            addWarning(e);
        }
    }
}

