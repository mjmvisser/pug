// Generated by CoffeeScript 1.3.3
var buildArgv;

buildArgv = function() {
    var argv, prop, _i, _len, _ref;
    // Qt.resolvedUrl returns a URL relative to this script, not the qml
    argv = [this.script];
    _ref = this.properties;
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
        prop = _ref[_i];
        if (!prop.input && !prop.output && this[prop.objectName] !== null && typeof this[prop.objectName] !== "undefined") {
            if (this[prop.objectName] !== false && prop.noFlag !== true) {
                var flagPrefix = "--";
                if (typeof prop.flagPrefix !== "undefined")
                    flagPrefix = prop.flagPrefix;
                    
                argv.push(flagPrefix + prop.objectName.replace(/^__/, ""));
            }
            if (this[prop.objectName] !== true && this[prop.objectName] !== false) {
                argv.push(this[prop.objectName]);
            }
        }
    }
    return argv;
};
