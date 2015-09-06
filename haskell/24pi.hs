import System.Environment
import Data.List
import Data.Ratio

main = do
    args <- getArgs
    do24p args

do24p l@[a,b,c,d] = print $ make24p (map (\x -> (read x::Integer)%1) l)
do24p _ = print "args error"

make24p :: [Rational] -> [String]
make24p l = foldr (++) [] [ searchRes 24 3 x [] [] | x <- permutations l ]

-- searchRes target numOfOps nums calcStack rp
searchRes :: Rational -> Int -> [Rational] -> [Rational] -> String -> [String]
searchRes target 0 [] calcStack rp
    |target == head calcStack = [rp]
    |otherwise = []
searchRes target n (x:y:ys) [] [] = searchRes target n ys [y,x] ((show (numerator y)) ++ (show (numerator x)))
searchRes target n (x:xs) [s] rp = searchRes target n xs [x,s] ((show (numerator x)) ++ rp)
searchRes target n nums@(x:xs) (s1:s2:ss) rp = (searchRes target n xs (x:s1:s2:ss) ((show (numerator x)) ++ rp))
                                        ++ (searchRes target (n-1) nums ((s1+s2):ss) ("+" ++ rp))
                                        ++ (searchRes target (n-1) nums ((s1-s2):ss) ("-" ++ rp))
                                        ++ (searchRes target (n-1) nums ((s1*s2):ss) ("*" ++ rp))
                                        ++ (if s2 /= 0 then searchRes target (n-1) nums ((s1/s2):ss) ("/" ++ rp) else [])
searchRes target n [] (s1:s2:ss) rp = (searchRes target (n-1) [] ((s1+s2):ss) ("+" ++ rp))
                                        ++ (searchRes target (n-1) [] ((s1-s2):ss) ("-" ++ rp))
                                        ++ (searchRes target (n-1) [] ((s1*s2):ss) ("*" ++ rp))
                                        ++ (if s2 /= 0 then searchRes target (n-1) [] ((s1/s2):ss) ("/" ++ rp) else [])
