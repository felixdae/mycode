module PnC
(combs
,combs2
,permus)

where

import Data.List (tails)

combs 0 _ = [[]]
combs n xs = [ y:ys | y:xs' <- tails xs, ys <- combs (n-1) xs']

combs2 0 x = [([], x)]
combs2 n [] = []
combs2 n (x:xs) = gc ++ hc
    where
    gc = [(x:a, b) | (a,b) <- combs2 (n-1) xs]
    hc = [(a, x:b) | (a,b) <- combs2 n xs]

permus 0 _ = [[]]
