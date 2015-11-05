module Knight
(Path(..),
Dsf,
Pos,
moveKnight,
inMany,
tryMany,
nextPos
)
where
import Control.Monad
import Data.List
import Debug.Trace

type KnightPos = (Int, Int)

moveKnight :: KnightPos -> [KnightPos]
moveKnight (c, r) = do
    (c', r') <- [(c + 2, r - 1), (c + 2, r + 1), (c - 2, r - 1), (c - 2, r + 1),
                (c + 1, r - 2), (c + 1, r + 2), (c - 1, r - 2), (c - 1, r + 2)]
    guard (c' `elem` [1..8] && r' `elem` [1..8])
    return (c', r')

inMany :: Int -> KnightPos -> [KnightPos]
inMany x start = return start >>= foldr (<=<) return (replicate x moveKnight)

type Pos = (Int, Int)
type Dsf = [Pos]
data Path a = Path a deriving (Show)

instance Functor Path where
    fmap = liftM

instance Applicative Path where
    pure = return
    (<*>) = ap

instance Monad Path where
    return = Path
    Path x >>= f = f x

move :: Pos -> Int -> Pos
move (x, y) 1 = (x + 1, y + 2)
move (x, y) 2 = (x + 2, y + 1)
move (x, y) 3 = (x + 2, y - 1)
move (x, y) 4 = (x + 1, y - 2)
move (x, y) 5 = (x - 1, y - 2)
move (x, y) 6 = (x - 2, y - 1)
move (x, y) 7 = (x - 2, y + 1)
move (x, y) 8 = (x - 1, y + 2)

legalMove :: Dsf -> Pos -> Bool
legalMove p (x, y)
    |not (x `elem` [1..8] && y `elem` [1..8]) = False
    |(x, y) `elem` p = False
    |otherwise = True

makeMove :: Dsf -> Int -> Pos
makeMove p@(h:hs) s = foldl (\acc d -> if acc /= h then acc else (
                    if legalMove p (move h d) then (move h d) else acc
                    )) h [s+1..8]
gd :: Pos -> Pos -> Int
gd f t
    | move f 1 == t = 1
    | move f 2 == t = 2
    | move f 3 == t = 3
    | move f 4 == t = 4
    | move f 5 == t = 5
    | move f 6 == t = 6
    | move f 7 == t = 7
    | move f 8 == t = 8

back :: Dsf -> Dsf
back path@(p:pn:ps)
    |np /= pn = {-trace (show np)-} (np:pn:ps)
    |otherwise = {-trace "back"-} back (pn:ps)
    where
        np = makeMove (pn:ps) (gd pn p)

nextPos :: Dsf -> Path Dsf
nextPos path@(p:ps)
    |np /= p = {-trace (show np)-} Path (np:path)
    |otherwise = {-trace "back"-} return $ back path
    where
        np = makeMove path 0

tryMany :: Int -> Pos -> Path Dsf
tryMany x start = return [start] >>= foldr (<=<) return (replicate x nextPos)
