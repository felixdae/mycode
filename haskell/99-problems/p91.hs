import Knight
import Data.List
import Debug.Trace (trace)

knightTravel :: Pos -> Dsf -> Int -> Int -> Path BoardPath
knightTravel s pf row col = do
    bp <- (return (BoardPath row col pf) >>= nextPos)
    let path = getPath bp
    -- if {- trace (show (head path)) -} head path == s then return path else knightTravel s path
    if length path == row*col then return (BoardPath row col path) else knightTravel s path row col

check :: Dsf -> Bool
check [_] = True
check ((x,y):(a,b):qs)
    |not ((abs (x-a) == 2 && abs (y-b) == 1) || (abs (x-a) == 1 && abs (y-b) == 2)) = False
    |otherwise = check ((a,b):qs)

main = do
    let (Path bp) = knightTravel (1,1) [(1,1)] 24 24
    -- case (getPath bp) of
    --     Path path -> do
    let path = getPath bp
    print $ all (\(x,y) -> elem x [1..8] && elem y [1..8]) path
    print $ all (\x -> length x == 1) (group path)
    print $ check path
    print . reverse $ path
    
