import Data.List
import Data.Ratio
-- import Debug.Trace (trace)

doCalc :: Ratio Int->Ratio Int->String->Ratio Int
-- doCalc _ Nothing _ = Nothing
-- doCalc Nothing _ _ = Nothing
doCalc a b "+" = a + b
doCalc a b "-" = a - b
doCalc a b "*" = a * b
doCalc a 0 "/" = 99999999
doCalc a b "/" = a / b

check24 :: [Ratio Int]->[String]->Bool
-- check24 a ("0":xs) = 24 == trace ((show a) ++ "0" ++ (show xs)) (doCalc (doCalc (doCalc (a !! 0) (a !! 1) (xs !! 0)) (a !! 2) (xs !! 1)) (a !! 3) (xs !! 2))
-- check24 a ("1":xs) = 24 == trace ((show a) ++ "1" ++ (show xs)) (doCalc (doCalc (a !! 0) (a !! 1) (xs !! 0)) (doCalc (a !! 2) (a !! 3) (xs !! 1)) (xs !! 2))
check24 a ("0":xs) = 24 == (doCalc (doCalc (a !! 0) (a !! 1) (xs !! 0)) (doCalc (a !! 2) (a !! 3) (xs !! 1)) (xs !! 2))
check24 a ("1":xs) = 24 == (doCalc (doCalc (doCalc (a !! 0) (a !! 1) (xs !! 0)) (a !! 2) (xs !! 1)) (a !! 3) (xs !! 2))
check24 a ("2":xs) = 24 == (doCalc (a !! 0) (doCalc (a !! 1) (doCalc (a !! 2) (a !! 3) (xs !! 0)) (xs !! 1)) (xs !! 2))

check24 a ("3":xs) = 24 == (doCalc (doCalc (a !! 0) (doCalc (a !! 1) (a !! 2) (xs !! 0)) (xs !! 1)) (a !! 3) (xs !! 2))
check24 a ("4":xs) = 24 == (doCalc (a !! 0) (doCalc (doCalc (a !! 1) (a !! 2) (xs !! 0)) (a !! 3) (xs !! 1)) (xs !! 2))

-- main = print (check24 [1,5,3,6] ["0","+","*","+"])
main = print [(p,op) | p <- (permutations [3, 3, 7, 7]), op <- [[x,y,z,w] | x <- ["0","1","2","3","4"], y <- ["+", "-", "*", "/"], z <- ["+", "-", "*", "/"], w <- ["+", "-", "*", "/"]], check24 p op]
