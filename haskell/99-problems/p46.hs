import Lr

table :: (Bool->Bool->Bool)->String
table f = printBinary f [True, False]

printBinary :: (Show a)=>(a->a->a)->[a]->String
printBinary f domain = concatMap (++"\n") [printBinaryInstance f x y|x<-domain, y<-domain]

printBinaryInstance :: (Show a)=>(a->a->a)->a->a->String
printBinaryInstance f x y = show x ++ " " ++ show y ++ " " ++ show (f x y)


main = do
    putStr $ table (\a b -> (and' a (or' a b)))
    putStr $ table (\a b -> (a `and'` (a `or'` not b)))
