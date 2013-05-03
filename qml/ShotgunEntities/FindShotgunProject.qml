import Pug 1.0

ShotgunFind {
    id: self
    entityType: "Project"
    fields: ["name", "sg_frame_rate", 
             "sg_image_resolution__in_", "sg_image_resolution__render_", 
             "sg_image_resolution__low_", "sg_image_resolution__low_render_"]
    limit: project.count
    
    inputs: [
        Input { name: "project" },
        Input { name: "__projectFilter" }
    ]

    property Node project
    property Node __projectFilter: projectFilter
    
    ShotgunFilterByField {
        id: projectFilter
        name: "projectFilter"
        branch: project
        shotgunField: "name"
        field: "PROJECT"
    }
    
    filters: {
        return [__projectFilter.details[0].filter];            
    }
}