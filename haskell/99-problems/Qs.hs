module Qs
(quickSort)
where

quickSort cmp [] = []
quickSort cmp (t:ts) =
    let sl = quickSort cmp [x|x<-ts, cmp x t]
        bl = quickSort cmp [x|x<-ts, not $ cmp x t]
    in sl ++ [t] ++ bl
