import Pug 1.0

ShotgunFind {
    id: self
    entityType: "Sequence"
    fields: ["code", "project"]
    limit: sequence.count
    
    inputs: [
        Input { name: "sequence" },
        Input { name: "sg_project" },
        Input { name: "__filters" }
    ]

    property Node sequence
    property Node sg_project
    property Node __filters: mergeFilters


    ShotgunFilterByField {
        id: codeFilter
        name: "codeFilter"
        shotgunField: "code"
        field: "SEQUENCE"
        branch: sequence
    }

    ShotgunFilterByEntity {
        id: projectFilter
        name: "projectFilter"
        shotgunField: "project"
        entity: sg_project
    }
    
    MergeShotgunFilters {
        id: mergeFilters
        name: "mergeFilters"
        filters: [codeFilter, projectFilter]
    }
    
    filters: {
        try {
            return [__filters.details[0].filter];            
        } catch (e) {
            addWarning(e);
        }
    }
}