import Data.List
import qualified Data.Set as S
import Debug.Trace (trace)

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
        in1Cell (a,b) = a>=0 && a<3 && b>=0 && b<3
        in2Cell (a,b) = a>=3 && a<6 && b>=0 && b<3
        in3Cell (a,b) = a>=6 && a<9 && b>=0 && b<3
        in4Cell (a,b) = a>=0 && a<3 && b>=3 && b<6
        in5Cell (a,b) = a>=3 && a<6 && b>=3 && b<6
        in6Cell (a,b) = a>=6 && a<9 && b>=3 && b<6
        in7Cell (a,b) = a>=0 && a<3 && b>=6 && b<9
        in8Cell (a,b) = a>=3 && a<6 && b>=6 && b<9
        in9Cell (a,b) = a>=6 && a<9 && b>=6 && b<9
sameRow (r,c) layout = [z | ((x,y),z) <- layout, x == r]
sameCol (r,c) layout = [z | ((x,y),z) <- layout, y == c]
sameCell (r,c) layout = [z | ((x,y),z) <- layout, isSameCell (r,c) (x,y)]

-- getBlankPos layout = toList (difference (fromList [(x,y) | x <- [0..8], y <- [0..8]]) (fromList (map (\(p,_)->p) layout)))
getBlankPos layout = [(x,y) | x <- [0..8], y <- [0..8]] \\ map fst layout

-- getCandidate layout = map (\x -> (x, toList (difference (difference (difference (fromList [1..9]) (fromList (sameRow x layout))) (fromList (sameCol x layout))) (fromList (sameCell x layout))))) $ getBlankPos layout
getCandidate layout = {- trace (show $ getBlankPos layout)-} map (\x -> (x, (([1..9] \\ sameRow x layout) \\ sameCol x layout) \\ sameCell x layout)) $ getBlankPos layout

-- searchCandidate [] = error "no fix condidate"
-- searchCandidate (((x,y),[n]):cs) = ((x,y),n)
-- searchCandidate (_:cs) = searchCandidate cs

searchCandidate :: [((Int, Int), [Int])] -> ((Int, Int), [Int])
searchCandidate [] = error "empty candidates"
searchCandidate candidates = {- trace (show candidates ++ "00000") -}foldr (\a@(_, e) b@(_, acc) -> if length e < length acc then a else b) (head candidates) candidates

updateCandidate _ [] = []
updateCandidate ((r,c),n) (((x,y),zs):cs)
    -- |r == x = ((x,y),toList (difference (fromList zs) (fromList [n]))):(updateCandidate ((r,c),n) cs)
    -- |c == y = ((x,y),toList (difference (fromList zs) (fromList [n]))):(updateCandidate ((r,c),n) cs)
    -- |isSameCell (r,c) (x,y) = ((x,y),toList (difference (fromList zs) (fromList [n]))):(updateCandidate ((r,c),n) cs)
    |(r == x) && (c == y) = updateCandidate ((r,c),n) cs
    |isSameCell (r,c) (x,y) || (r == x) || (c == y) = if null zsdn then updateCandidate ((r,c),n) cs else ((x,y),  zsdn):updateCandidate ((r,c),n) cs
    |otherwise = ((x,y),zs):updateCandidate ((r,c),n) cs
        where
        zsdn = zs \\ [n]

mergeCandidates pcdt = S.toList . S.fromList $ concatMap snd pcdt

-- buggy deepUpdateCandidatesOfOne ((x,y),_) candidates = deepUpdateCandidates candidates $ filter (\ ((r,c),_) -> (r == x && c /= y) || (r /= x && c == y) || (isSameCell (r,c) (x,y) && (r /= x || c /= y))) candidates
deepUpdateCandidates _ [] = []
deepUpdateCandidates candidates (psb@(pos,_):psbs)
    | length (sameRowDiff psb) > 1 = {-trace (show candidates ++ "lllll" ++ show psb ++ "111" ++ show (sameRowDiff psb))-} []
    -- | length (sameRowDiff psb) == 1 = {-trace (show psb ++ "111" ++ show (sameRowDiff psb))-} (pos, sameRowDiff psb):psbs
    | length (sameRowDiff psb) == 1 = {-trace (show psb ++ "111" ++ show (sameRowDiff psb))-} (pos, sameRowDiff psb):deepUpdateCandidates candidates psbs
    | length (sameColDiff psb) > 1 = {-trace (show psb ++ "222" ++ show (sameColDiff psb))-} []
    --- | length (sameColDiff psb) == 1 = {-trace (show psb ++ "222" ++ show (sameColDiff psb))-} (pos, sameColDiff psb):psbs
    | length (sameColDiff psb) == 1 = {-trace (show psb ++ "222" ++ show (sameColDiff psb))-} (pos, sameColDiff psb):deepUpdateCandidates candidates psbs
    | length (sameCellDiff psb) > 1 = {-trace (show psb ++ "333" ++ show (sameCellDiff psb))-} []
    -- | length (sameCellDiff psb) == 1 = {-trace (show psb ++ "333" ++ show (sameCellDiff psb))-} (pos, sameCellDiff psb):psbs
    | length (sameCellDiff psb) == 1 = {-trace (show psb ++ "333" ++ show (sameCellDiff psb))-} (pos, sameCellDiff psb):deepUpdateCandidates candidates psbs
    | otherwise = psb : deepUpdateCandidates candidates psbs
        where
        sameRowDiff ((x,y),cs) = cs \\ mergeCandidates (filter (\ ((r,c),_) -> r == x && c /= y) candidates)
        sameColDiff ((x,y),cs) = cs \\ mergeCandidates (filter (\ ((r,c),_) -> r /= x && c == y) candidates)
        sameCellDiff ((x,y),cs) = cs \\ mergeCandidates (filter (\ ((r,c),_) -> isSameCell (r,c) (x,y) && (r /= x || c /= y)) candidates)

solveSudoku layout candidates
    |length layout == 81 = [layout]
    |length candidates + length layout < 81 = []
    -- |otherwise = {- trace (show newFix) -}solveSudoku (newFix:layout) (updateCandidate newFix candidates)
    -- |otherwise = {-trace (show layout ++ "\n" ++ show candidates) -}concatMap (\x -> solveSudoku (x:layout) ({-trace "hhhhhhhhhhhhh"-} deepUpdateCandidates (updc x) (updc x))) newFix
    |otherwise = {-trace (show layout ++ "\n" ++ show candidates) -}concatMap (\x -> solveSudoku (x:layout) ({-trace "hhhhhhhhhhhhh"-} updc x)) newFix
        where
        -- newFix = {- trace (show searchRes) -}fst searchRes, head (snd searchRes))
        updc x = updateCandidate x candidates
        newFix = [((x,y),z) | ((x,y),zs) <- [searchRes], z<-zs]
            where
            searchRes = searchCandidate candidates

calcSudoku layout = {-trace (show "fdsafsafsdaffsa" ++ show (getCandidate layout))-} solveSudoku layout $ deepUpdateCandidates oriCanditates oriCanditates
    where
    oriCanditates = (getCandidate layout)

collectRes :: [[((Int,Int),Int)]]->String
collectRes = concatMap ((++"\n") . collectOneRes 0)

collectOneRes :: Int->[((Int,Int),Int)]->String
collectOneRes n layout
    |n > 80 = ""
    |otherwise = show (head [z | ((x,y),z) <- layout, (9*x+y)==n]) ++ (if mod (n+1) 9 == 0 then "\n" else " ") ++ collectOneRes (n+1) layout

makeAPuzzle ln = doMakePuzzle (zip [0..] ln)
    where
    doMakePuzzle [] = []
    doMakePuzzle ((o,n):zs)
        |n == '.' = doMakePuzzle zs
        |otherwise = ((div o 9, mod o 9), read [n]::Int):doMakePuzzle zs

main = do
    contents <- getContents
    let allInput = map makeAPuzzle (lines contents)
    -- input like 1 2 3\n1 3 5..
    -- putStr $ collectRes (calcSudoku (map (\(r:' ':c:' ':[n]) -> (((read [r]::Int) - 1, (read [c]::Int) - 1), read [n]::Int)) (lines contents)))
    mapM (putStr . collectRes . calcSudoku) allInput
