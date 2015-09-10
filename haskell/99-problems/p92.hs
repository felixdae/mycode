import Data.List
import Qs

checkPermu l = doCheckPermu (quickSort (\x y -> x<y) l)
    where
    doCheckPermu [] = True
    doCheckPermu [x] = True
    doCheckPermu (x:y:ys)
        |x /= y = doCheckPermu (y:ys)
        |otherwise = False

vonKoch edges = doVonKoch (permutations [1..(length edges)+1]) edges
    where
    doVonKoch [] edges = []
    doVonKoch (p:ps) edges = if solt then p else doVonKoch ps edges
        where solt = checkPermu (map (\(s,e) -> abs ((p !! (s-1)) - (p !! (e-1)))) edges)

main = do
    print $ vonKoch [(1,6),(2,6),(3,6),(4,6),(5,6),(5,7),(5,8),(8,9),(5,10),(10,11),(11,12),(11,13),(13,14)]
    -- [7,11,8,9,12,4,10,6,5,3,13,2,1,14]

