import Pug 1.0

ShotgunFind {
    id: self
    entityType: "Shot"
    limit: shot.count
    fields: ["code", "project", "sg_sequence", "sg_head_in", "sg_tail_out"]
    
    inputs: [
        Input { name: "shot" },
        Input { name: "sg_project" },
        Input { name: "sg_sequence" },
        Input { name: "__filters" }
    ]
    
    property Node shot
    property Node sg_project
    property Node sg_sequence
    property Node __filters: mergeFilters 

    ShotgunFilterByField {
        id: codeFilter
        name: "codeFilter"
        shotgunField: "code"
        field: "SHOT"
        branch: shot
    }
    
    ShotgunFilterByEntity {
        id: projectFilter
        name: "projectFilter"
        shotgunField: "project"
        entity: sg_project
    }
    
    ShotgunFilterByEntity {
        id: sequenceFilter
        name: "sequenceFilter"
        shotgunField: "sg_sequence"
        entity: sg_sequence
    }

    MergeShotgunFilters {
        id: mergeFilters
        name: "mergeFilters"
        filters: [codeFilter, projectFilter, sequenceFilter]
    }    

    filters: {
        return [__filters.details[0].filter];            
    }
}
