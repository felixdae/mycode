import Knight
import Data.List
import Debug.Trace (trace)

knightTravel :: Pos -> Dsf -> Path Dsf
knightTravel s pf = do
    path <- (return pf >>= nextPos)
    -- if {- trace (show (head path)) -} head path == s then return path else knightTravel s path
    if length path == 64 then return path else knightTravel s path

check :: Dsf -> Bool
check [_] = True
check ((x,y):(a,b):qs)
    |not ((abs (x-a) == 2 && abs (y-b) == 1) || (abs (x-a) == 1 && abs (y-b) == 2)) = False
    |otherwise = check ((a,b):qs)

main = do
    case knightTravel (1,1) [(1,1)] of
        Path path -> do
            print $ all (\(x,y) -> elem x [1..8] && elem y [1..8]) path
            print $ all (\x -> length x == 1) (group path)
            print $ check path
            print . reverse $ path
    
