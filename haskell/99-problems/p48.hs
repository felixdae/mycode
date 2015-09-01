import Lr

tablen ::Integer->([Bool]->Bool)->String
tablen n f = printMulti n f [True, False]

printMulti:: (Show a)=>Integer->([a]->a)->[a]->String
printMulti n f domain = concatMap (++"\n") [printMultiInstance f ln|ln <- enumerateN n domain]

printMultiInstance:: (Show a)=>([a]->a)->[a]->String
printMultiInstance f ln = concatMap ((++" ").show) ln ++ show (f ln)

main = do
    putStr $ tablen 3 (\[a,b,c] -> a `and'` (b `or'` c) `equ'` a `and'` b `or'` a `and'` c)
