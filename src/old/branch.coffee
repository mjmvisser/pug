__escape = (str) ->
  # just the "." for now
  str.replace(".", "\\.")

fieldNamesFromPattern = (pattern) ->
  # given a path specifier in the form "...{a}..{b}..."
  # get a list of field names in the form [..., "a", "b", ...]
  fieldNames = []
  re = ///
      \{    # begin field
      (\w+) # field name
      \}    # end field
    ///g
  while m = re.exec @pattern
    fieldNames.push m[1]
  
  return fieldNames

init = () ->
  if @parent?.children?
    @__downstream = (node for node in @parent.children when node.className.indexOf("Branch") == 0 and node.input is this)
  else
    @__downstream = []
    
  Array::push.apply @__downstream, (node for node in @children when node.className.indexOf("Branch") == 0)

  # finally, check that we have all the fields we need
  for fieldName in fieldNamesFromPattern pattern
    if not getField fieldName
      throw new Error("Can't find field #{fieldName} in #{@objectName}")

  for child in @children
    child.init?()
    
getField = (fieldName) ->
  # check our fields first
  for field in @properties
    if field.name == fieldName
      return field

  # find (grand)parent with fields
  p = @parent
  until p?.className.indexOf("Branch") == 0 or p?.className.indexOf("Config") == 0 or !p?
    p = p.parent
    
  if p?.getField?
    return p.getField fieldName
  else
    return null

__zip = (keys, values) ->
  result = {}
  result[keys[i]] = values[i] for i in [0..keys.length]
  result

__match = (path, pattern) ->
  # convert a pattern into a regular expression string that parses a path
  parseRegexp = __escape(pattern).replace ///
      \{    # begin field
      (\w+) # field name
      \}    # end field
    ///g,
    (match, fieldName, offset, s) =>
      "(" + getField(fieldName)?.regexp + ")"

  # match against input path
  r = RegExp("^" + if @exactMatch then parseRegexp + "$" else parseRegexp)
  matches = r.exec path
  if matches?
    last = matches[0].length
    remainder = path[last..]
    fields = __zip fieldNamesFromPattern(pattern), matches[1..]
    if not @exactMatch and remainder
      fields["_"] = remainder
    return fields
  else
    return null

__getInput = () ->
  if @input?
    return input
  else
    # go up the parenting tree until we find a branch that is not a leaf
    p = @parent
    until (p?.__isBranch? or not p?) and not p?.exactMatch
      p = p.parent
      
    return p

parse = (path) ->
  if path[0] == "/"
    input = __getInput()
    if input?
      # recurse
      fields = input.parse path

      if fields?
        remainder = fields["_"]

        if remainder?
          delete fields["_"]
          
          # match the remainder
          our_fields = __match remainder, @pattern

          # convert fields
          for fieldName in fieldNamesFromPattern(@pattern)
              our_fields[fieldName] = getField(fieldName).convert(our_fields[fieldName])
  
          if our_fields?        
            # merge input fields with our own
            for own key, value of our_fields
              fields[key] = value
    else
      # root
      fields = __match path, @pattern
          
    return fields
  else
    throw new Error("Can't match a relative path")

formatPatternForSprintf = (pattern) ->
  # convert a pattern into an sprintf format string using named parameters
  return pattern.replace ///
      \{    # begin field
      (\w+) # field name
      \}    # end field
    ///g,
    (match, fieldName, offset, s) =>
      getField(fieldName)?.format?.replace("%", "%(" + fieldName + ")")
  
map = (fields) ->
  # map parent
  input = __getInput()
  if input?
    mappedParent = input.map fields
  else
    mappedParent = ""

  formattedPattern = formatPatternForSprintf @pattern
  
  # and concatenate
  return mappedParent + Sprintf.sprintf formattedPattern, fields

mapPattern = (pattern, fields) ->
  # map parent
  input = __getInput()
  if input?
    mappedParent = input.map fields
  else
    mappedParent = ""

  formattedPattern = formatPatternForSprintf pattern

  # and concatenate
  return mappedParent + Sprintf.sprintf formattedPattern, fields

mkpath = (fields) ->
  if @__downstream.length
    # recurse on all our children and return true if all succeeded
    (node.mkpath fields for node in @__downstream).every (x) -> x?
  else
    # make the directories
    # TODO: feedback on failure
    path = map(fields)
    dir = path.replace /[^/]*$/, ''
    Util.mkpath dir

onUpdate = (env) ->
  @path = map(env)
  @UpdateOperation.updated(Operation.Finished)
