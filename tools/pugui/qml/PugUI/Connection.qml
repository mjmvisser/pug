import QtQuick 2.0

Canvas {
    id: self
    property Item source
    property Item sink
    
    renderTarget: Canvas.FramebufferObject
    
    onPaint: {
        var ctx = self.getContext("2d");
        
        console.log("drawing");
        
        if (source && sink) {
            console.log("ok, drawing line from " + source + " to " + sink);
            ctx.save();
            
            ctx.strokeStyle = Qt.rgba(0, 0, 0, 1);
            ctx.globalCompositeOperation = "source-over";
            ctx.lineWidth = 1;
            ctx.beginPath();
            ctx.moveTo(source.x+source.width/2, source.y+source.height);
            ctx.lineTo(sink.x+sink.width/2, sink.y);
            ctx.stroke();
            
            ctx.restore();
        }
    }
}
