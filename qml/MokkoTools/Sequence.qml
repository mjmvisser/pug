import Pug 1.0
import ShotgunEntities 1.0

File {
    id: self
    property MakeThumbnail thumbnail: makeThumbnail
    property MakeThumbnail filmstrip: makeFilmstrip
    
    MakeThumbnail {
        id: makeThumbnail
        name: "makeThumbnail"
        input: self
        active: true
    }

    MakeThumbnail {
        id: makeFilmstrip
        name: "makeFilmstrip"
        input: self
        filmstrip: true
        active: true
    }
    
}
