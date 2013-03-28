import Pug 1.0

ShotgunEntity {
    shotgunEntity: "Step"
    
    property string step

     params: [
        Param { name: "step" }
    ]
   
    ShotgunField {
        name: "code"
        shotgunField: "short_name"
        value: step
    }
}
