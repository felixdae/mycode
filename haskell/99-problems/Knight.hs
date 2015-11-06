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

-- makeMove :: Dsf -> Int -> Pos
-- makeMove p@(h:hs) s = foldl (\acc d -> if acc /= h then acc else (
--                     if legalMove p (move h d) then (move h d) else acc
--                     )) h [s+1..8]
-- 
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
    | otherwise = error "error"

-- back :: Dsf -> Dsf
-- back path@(p:pn:ps)
--     |np /= pn = {-trace (show np)-} (np:pn:ps)
--     |otherwise = {-trace "back"-} back (pn:ps)
--     where
--         np = makeMove (pn:ps) (gd pn p)
-- 
-- nextPos :: Dsf -> Path Dsf
-- nextPos path@(p:ps)
--     |np /= p = {-trace (show np)-} Path (np:path)
--     |otherwise = {-trace "back"-} return $ back path
--     where
--         np = makeMove path 0

chooseMin :: Dsf -> [Pos] -> Maybe Pos
chooseMin _ [] = Nothing
chooseMin path@(p:ps) candis = Just (foldr (\acc c -> (choice c acc)) (head candis) candis)
    where
        choice a b
            |la < lb = a
            |la > lb = b
            |otherwise = if (gd p a) < (gd p b) then a else b
                where
                    la = length (filter (legalMove path) [move a x | x<-[1..8]])
                    lb = length (filter (legalMove path) [move b x | x<-[1..8]])


makeMove :: Dsf -> Int -> Maybe Pos
makeMove p@(h:hs) s = chooseMin p (filter (legalMove p) [move h x | x<-[1..8]])

-- makeMove p@(h:hs) s = foldl (\acc d -> case acc of
--                         (Just _) -> acc
--                         Nothing -> if legalMove p (move h d) then Just (move h d) else acc) Nothing [s+1..8]

back :: Dsf -> Dsf
back path@(p:pn:ps) = 
    -- |np /= pn = {-trace (show np)-} (np:pn:ps)
    -- |otherwise = {-trace "back"-} back (pn:ps)
    case mnp of
        (Just np) -> {-trace (show np)-} (np:pn:ps)
        Nothing -> {-trace "back"-} back (pn:ps)
    where
        mnp = makeMove (pn:ps) (gd pn p)

nextPos :: Dsf -> Path Dsf
nextPos path@(p:ps) = 
    -- |np /= p = {-trace (show np)-} Path (np:path)
    -- |otherwise = {-trace "back"-} return $ back path
    case mnp of
        (Just np) -> {-trace (show np)-} Path (np:path)
        Nothing -> {-trace "back"-} return $ back path
    where
        mnp = makeMove path 0

tryMany :: Int -> Pos -> Path Dsf
tryMany x start = return [start] >>= foldr (<=<) return (replicate x nextPos)
