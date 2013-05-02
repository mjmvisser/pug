import Pug 1.0

ShotgunFind {
    id: self
    entityType: "PublishEvent"
    limit: 1 
    fields: ["project", "sg_version", "sg_variation", "sg_path", "sg_link", "sg_step"]
    order: [{field_name: "sg_version", direction: "desc"}]

    property Node sg_project
    property Node sg_entity
    property Node sg_step
    property Node __filters: mergeFilters

    inputs: [
        Input { name: "sg_project" },
        Input { name: "sg_entity" },
        Input { name: "sg_step" },
        Input { name: "__filters" }
    ]

    ShotgunFilterByEntity {
        id: projectFilter
        name: "projectFilter"
        shotgunField: "project"
        entity: sg_project
    }

    ShotgunFilterByEntity {
        id: entityFilter
        name: "entityFilter"
        shotgunField: "sg_link"
        entity: sg_entity
    }

    ShotgunFilterByEntity {
        id: stepFilter
        name: "stepFilter"
        shotgunField: "sg_step"
        entity: sg_step
    }
    
    MergeShotgunFilters {
        id: mergeFilters
        filters: [projectFilter, entityFilter, stepFilter]
    }
    
    filters: {
        try {
            return [__filters.details[0].filter];            
        } catch (e) {
            addWarning(e);
        }
    }
    
    
    UpdateOperation.onCookFinished: {
        for (var i = 0; i < details.length; i++) {
            self.File.elements[i].pattern = details[i].entity.sg_path;
            self.File.elements[i].scan();
        }
    }
}

