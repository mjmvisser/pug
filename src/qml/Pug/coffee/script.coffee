buildArgv = () ->
  argv = [@script]
  for prop in @properties
    if not prop.input and not prop.output and @[prop.name]?
      if @[prop.name] != false and prop.noFlag != true
        flagPrefix = if !prop.flagPrefix? then "--" else prop.flagPrefix
        argv.push "#{flagPrefix}#{prop.objectName.replace(/^__/, "")}";
      if @[prop.name] != true and @[prop.name] != false        
        argv.push this[prop.objectName]
    
  #console.log "buildArgv built #{JSON.stringify argv}"
    
  return argv

parentPath = () ->
  branch = @
  while branch?.parent?.className? != "Branch"
    branch = branch.parent
    
  if branch?.elements.length > 0
    return branch.elements[0].path
  else
    return ""

generatePath = (name, index, frameSpec, ext) ->
  if frameSpec
    return "#{parentPath()}tmp/#{name}_#{index}.#{frameSpec}.#{ext}"
  else
    return "#{parentPath()}tmp/#{name}_#{index}.#{ext}"
    
    
    
