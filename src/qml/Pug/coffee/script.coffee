buildArgv = () ->
  argv = [Qt.resolvedUrl(@script).replace("file://", "")]
  for prop in @properties
    if not prop.input and not prop.output
      argv.push "--#{prop.objectName}"
      argv.push this[prop.objectName]
    
  #console.log "buildArgv built #{JSON.stringify argv}"
    
  return argv
