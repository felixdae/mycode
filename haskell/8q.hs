import Data.List

eightQueen

appendOne okSet

findPos okSet
    | length okSet == 8 = okSet
    | otherwise = findPos (appendOne okSet)
