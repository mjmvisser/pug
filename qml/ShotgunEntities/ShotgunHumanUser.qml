import Pug 1.0

ShotgunEntity {
    id: userEntity
    name: "userEntity"
    shotgunEntity: "HumanUser"
    shotgunFields: [
        loginField
    ]

    output: true

    ShotgunField {
        id: loginField
        name: "loginField"
        shotgunField: "login"
        required: true
        field: "USER"
        source: userEntity.parent
    }
}