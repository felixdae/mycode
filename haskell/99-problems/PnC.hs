module PnC
(combs
,combs2
,permus
,permuAll)

where

import Data.List (tails)
-- import Debug.Trace (trace)

combs 0 _ = [[]]
combs n xs = [ y:ys | y:xs' <- tails xs, ys <- combs (n-1) xs']

combs2 0 x = [([], x)]
combs2 n [] = []
combs2 n (x:xs) = gc ++ hc
    where
    gc = [(x:a, b) | (a,b) <- combs2 (n-1) xs]
    hc = [(a, x:b) | (a,b) <- combs2 n xs]

-- permus [1..n]
-- findFirstInc找出第一个升序位置
permus l = reOrder (findFirstInc [] l)
    where
    findFirstInc acc [] = (acc,[])
    findFirstInc acc [x] = (acc ++ [x], [])
    findFirstInc acc (x:xs@(y:ys))
        |x >= y = findFirstInc (acc ++ [x]) xs
        |otherwise = (acc ++ [x], xs)  

    reOrder (hl, []) = hl
    reOrder (hl, (t:ts)) = findPos hl t [] ++ ts

    -- findPos [] t acc = reverse acc ++ [t]
    -- findPos hl [] acc = []
    findPos (x:xs) t acc
        | x >= t = (findPos xs t (x:acc))
        | otherwise = (reverse xs) ++ [t] ++ acc ++ [x]

eqList :: (Eq a) => [a]->[a]->Bool
eqList [] [] = True
eqList (x:xs) [] = False
eqList [] (x:xs) = False
eqList (x:xs) (y:ys)
    |x == y = eqList xs ys
    |otherwise = False


permuAll l acc
    |eqList l nextp = acc
    |otherwise = permuAll nextp (nextp:acc)
        where
        nextp = permus l
