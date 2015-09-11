import Data.List
-- import Data.Set hiding (map)
-- import Debug.Trace (trace)

isSameCell rc xy
    |in1Cell rc && in1Cell xy = True
    |in2Cell rc && in2Cell xy = True
    |in3Cell rc && in3Cell xy = True
    |in4Cell rc && in4Cell xy = True
    |in5Cell rc && in5Cell xy = True
    |in6Cell rc && in6Cell xy = True
    |in7Cell rc && in7Cell xy = True
    |in8Cell rc && in8Cell xy = True
    |in9Cell rc && in9Cell xy = True
    |otherwise = False
        where
        in1Cell (a,b) = (a>=0 && a<3 && b>=0 && b<3)
        in2Cell (a,b) = (a>=3 && a<6 && b>=0 && b<3)
        in3Cell (a,b) = (a>=6 && a<9 && b>=0 && b<3)
        in4Cell (a,b) = (a>=0 && a<3 && b>=3 && b<6)
        in5Cell (a,b) = (a>=3 && a<6 && b>=3 && b<6)
        in6Cell (a,b) = (a>=6 && a<9 && b>=3 && b<6)
        in7Cell (a,b) = (a>=0 && a<3 && b>=6 && b<9)
        in8Cell (a,b) = (a>=3 && a<6 && b>=6 && b<9)
        in9Cell (a,b) = (a>=6 && a<9 && b>=6 && b<9)
sameRow (r,c) layout = [z | ((x,y),z) <- layout, x == r]
sameCol (r,c) layout = [z | ((x,y),z) <- layout, y == c]
sameCell (r,c) layout = [z | ((x,y),z) <- layout, isSameCell (r,c) (x,y)]

-- getBlankPos layout = toList (difference (fromList [(x,y) | x <- [0..8], y <- [0..8]]) (fromList (map (\(p,_)->p) layout)))
getBlankPos layout = [(x,y) | x <- [0..8], y <- [0..8]] \\ (map (\(p,_)->p) layout)

-- getCandidate layout = map (\x -> (x, toList (difference (difference (difference (fromList [1..9]) (fromList (sameRow x layout))) (fromList (sameCol x layout))) (fromList (sameCell x layout))))) $ getBlankPos layout
getCandidate layout = map (\x -> (x, (([1..9] \\ (sameRow x layout)) \\ (sameCol x layout)) \\ (sameCell x layout))) $ getBlankPos layout

searchCandidate [] = error "no fix condidate"
searchCandidate (((x,y),[n]):cs) = ((x,y),n)
searchCandidate (_:cs) = searchCandidate cs

updateCandidate ((r,c),n) (((x,y),zs):cs)
    -- |r == x = ((x,y),toList (difference (fromList zs) (fromList [n]))):(updateCandidate ((r,c),n) cs)
    -- |c == y = ((x,y),toList (difference (fromList zs) (fromList [n]))):(updateCandidate ((r,c),n) cs)
    -- |isSameCell (r,c) (x,y) = ((x,y),toList (difference (fromList zs) (fromList [n]))):(updateCandidate ((r,c),n) cs)
    |isSameCell (r,c) (x,y) || (r == x) || (c == y) = ((x,y),  zs \\ [n]):(updateCandidate ((r,c),n) cs)
    |otherwise = (((x,y),zs):(updateCandidate ((r,c),n) cs))

solveSudoku layout candidates
    |length layout == 81 = layout
    |otherwise = {-trace (show newFix) -}solveSudoku (newFix:layout) (updateCandidate newFix candidates)
        where
        newFix = searchCandidate candidates

calcSudoku layout = solveSudoku layout (getCandidate layout)

collectRes :: Int->[((Int,Int),Int)]->String
collectRes n layout
    |n > 80 = ""
    |otherwise = show ([z | ((x,y),z) <- layout, (9*x+y)==n] !! 0) ++ (if (mod (n+1) 9 == 0) then "\n" else " ") ++ (collectRes (n+1) layout)

main = do
    contents <- getContents
    putStr $ collectRes 0 (calcSudoku (map (\(r:' ':c:' ':[n]) -> (((((read [r])::Int) - 1), (((read [c])::Int) - 1)), (read [n])::Int)) (lines contents)))
    -- print $ (calcSudoku (map (\(r:' ':c:' ':[n]) -> (((((read [r])::Int) - 1), (((read [c])::Int) - 1)), (read [n])::Int)) (lines contents)))
    -- print contents
