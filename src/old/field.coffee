# convert the given string to a value of the proper type
convert = (str) ->
  switch @type
    when "int" 
      parseInt(str, 10)
    when "float"
      parseFloat(str)
    else
      str
