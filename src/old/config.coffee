parse = (node, path) ->
  return findNode(node)?.parse path
 
map = (node, fields) ->
  return findNode(node)?.map fields

init = () ->
  for child in @children
    child.init?()

getField = (fieldName) ->
  for field in @properties
    if field.name == fieldName
      return field

  return null

makeDirs = (node, fields) ->
  return findNode(node)?.makeDirs fields
