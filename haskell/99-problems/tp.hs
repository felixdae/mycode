-- test permutations
import PnC
import Qs

main = do
    print $ length (permuAll (quickSort (<) [3,4,1,2,5]) [quickSort (<) [3,4,1,2,5]])
    -- print $ length (permuAll "12345" [])
    -- print $ permus [5,4,3,2,1]
