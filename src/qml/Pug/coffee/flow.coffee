mkpath = (env) ->
  # call mkpath on children return true if all return true
  (node.mkpath env for node in @children when node.mkpath?).every (x) -> x?
