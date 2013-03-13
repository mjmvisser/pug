import Pug 1.0
import Shotgun 1.0

Config {
    Field {
       objectName: "PROJECT"
       Shotgun.entity: "Project"
       Shotgun.field: "name"
    }
       
    Field {
        objectName: "SEQUENCE"
        Shotgun.entity: "Sequence"
        Shotgun.field: "code"
    }

    Field {
        objectName: "SHOT"
        Shotgun.entity: "Shot"
        Shotgun.field: "code"
    }

    Field {
        objectName: "STEP"
        Shotgun.entity: "Step"
        Shotgun.field: "name"
    }

    Field {
        objectName: "USER"
        Shotgun.entity: "HumanUser"
        Shotgun.field: "login"
    }

    Field {
        objectName: "VARIATION"
        Shotgun.entity: "CustomEntity01"
        Shotgun.field: "code"
    }
        
    Field {
        objectName: "VERSION"
        type: "int"
    }
    
    Field {
        objectName: "FRAMESPEC"
        regexp: "(?:%0?\\d+d)|(?:#+)|(?:[$]F\\d*)"
    }

    Field {
        objectName: "RES"
        values: ["2k", "2k_und"]
        Shotgun.entity: "CustomEntity02"
        Shotgun.field: "sg_res"
    }
    
    Field {
        objectName: "FORMAT"
        values: ["exr", "dpx", "jpg"]
        Shotgun.entity: "CustomEntity02"
        Shotgun.field: "sg_format"
    }
    
    Field {
        objectName: "EXT"
        Shotgun.entity: "CustomEntity02"
        Shotgun.field: "sg_ext"
    }
    
    contents: Network {
        Folder {
            id: root
        	objectName: "root"
        	pattern: "/usr/tmp/testconfig/prod/projects/"
        }
        
        Folder {
            id: project
        	objectName: "project"
        	pattern: "{PROJECT}/"
            input: root
        }
        
        Folder {
            id: sequence
        	objectName: "sequence"
        	pattern: "shots/{SEQUENCE}/"
        	input: project
        }
        
        Folder {
            id: shot
        	objectName: "shot"
        	pattern: "{SHOT}/"
        	input: sequence
        }
        
        Plate {
            id: plate
            objectName: "plate"
            pattern: "{DEPARTMENT}/"
            input: shot
            constraints: fields.DEPARTMENT === "plate"
        }
    }
}