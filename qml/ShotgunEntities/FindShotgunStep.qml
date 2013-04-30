import Pug 1.0

ShotgunFind {
    id: self
    entityType: "Step"
    limit: step.count
    fields: ["code"]

    inputs: [
        Input { name: "step" },
        Input { name: "__filters" }
    ]

    property Node step
    property Node __filters: mergeFilters
   
    ShotgunFilterByField {
        id: shortNameFilter
        name: "shortNameFilter"
        shotgunField: "short_name"
        field: "STEP"
        branch: step
    }
    
    MergeShotgunFilters {
        id: mergeFilters
        name: "mergeFilters"
        filters: [shortNameFilter]
    }
    
    filters: {
        try {
            return [__filters.details[0].filter];            
        } catch (e) {
            addWarning(e);
        }
    }
}
