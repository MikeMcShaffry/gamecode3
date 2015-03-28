
---------------------------------------------------------------------------
--  Utilities
---------------------------------------------------------------------------

Utils = { }


--
-- Make a first-level copy of a table (no depth-recursive) 
-- into a new table, or into another table (adding to existing elements)
--
-- in:
--   fromTable = table whose first-level elements are to be copied
--   toTable = table into which elements are copied; create a new table if nil
--   *Warning* elements of the same name already present in <toTable> are replaced.
-- return: <toTable> (or a new table) with copies of elements from <fromTable>
--
-- Examples:
--   nt = Utils.ShallowCopy(t) -- return a first-level clone of <t>
--   nt = Utils.ShallowCopy(t, t0) -- copy <t> elements into <t0>, return <t0>
--
-- Note:
--   <toTable>, if given, is modified; this is done (instead of copying 
--   its elements also to a third table) to make "inheritance" more efficient 
--   in 'pseudo-OO' programming.
-- 
--
function Utils.ShallowCopy(fromTable, toTable)
  toTable = toTable or {} -- new table if not given
  local inx = nil
  local val = nil
  repeat
    inx, val = next(fromTable, inx)
    if val then toTable[inx] = val end
    -- print("inx: ", inx, "val: ", val, "\n") -- (useful for debugging)
  until (inx == nil)
  return toTable
end


--
-- Call a function for every permutation of the numeric elements of a table,
-- starting from element <pos> (previous elements are left where they are)
-- Note: this function is destructive: elements are rearranged for efficiency
-- (minimal garbage creation), then replaced in their original position if
-- and when all permutations are done.
--
-- in:
--   t       = table with numeric elements
--   func(t) = function to be called for every permutation,
--             return a true value to exit from permutation loop
--   pos     = (default = 1) first element to be permutated
-- return:   
--   true = exit requested, false/nil = finished normally
--
function Utils.Permutate(t, func, pos)
  local exitRequest = false
  pos = pos or 1 -- start from element 1 if not given
  local size = table.getn(t)
  if pos == size then
    exitRequest = func(t)
  else
    local i = pos
    while (not exitRequest) and i <= size do
      t[pos], t[i] = t[i], t[pos] -- swap
      exitRequest = Utils.Permutate(t, func, pos + 1)
      t[pos], t[i] = t[i], t[pos] -- replace
      i = i + 1
    end
  end
  return exitRequest
end


