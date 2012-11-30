var buildFormatString;

buildFormatString = function(metadata) {
    var result = [];
    for (var key in metadata) {
        if (metadata.hasOwnProperty(key)) {
            result.push(key + "=" + metadata[key]);
        }
    }
    
    return result.join("\n");
}