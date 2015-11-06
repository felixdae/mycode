module Knight
(Path(..),
Dsf(..),
BoardPath(..),
Pos,
moveKnight,
inMany,
-- tryMany,
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

data BoardPath = BoardPath {
                getRow::Int,
                getCol::Int,
                getPath::[Pos]
                }
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

legalMove :: Dsf -> Int -> Int -> Pos -> Bool
legalMove p row col (x, y)
    |not (x `elem` [1..row] && y `elem` [1..col]) = False
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

chooseMin :: Dsf -> [Pos] -> Int -> Int -> Maybe Pos
chooseMin _ [] _ _ = Nothing
chooseMin path@(p:ps) candis row col = Just (foldr (\acc c -> (choice c acc)) (head candis) candis)
    where
        choice a b
            |la < lb = a
            |la > lb = b
            |otherwise = if (gd p a) < (gd p b) then a else b
                where
                    la = length (filter (legalMove path row col) [move a x | x<-[1..8]])
                    lb = length (filter (legalMove path row col) [move b x | x<-[1..8]])


makeMove :: Dsf -> Int -> Int -> Int -> Maybe Pos
makeMove p@(h:hs) s row col = chooseMin p (filter (legalMove p row col) [move h x | x<-[1..8]]) row col

-- makeMove p@(h:hs) s = foldl (\acc d -> case acc of
--                         (Just _) -> acc
--                         Nothing -> if legalMove p (move h d) then Just (move h d) else acc) Nothing [s+1..8]

back :: Dsf -> Int -> Int -> Dsf
back path@(p:pn:ps) row col = 
    -- |np /= pn = {-trace (show np)-} (np:pn:ps)
    -- |otherwise = {-trace "back"-} back (pn:ps)
    case mnp of
        (Just np) -> {-trace (show np)-} (np:pn:ps)
        Nothing -> {-trace "back"-} back (pn:ps) row col
    where
        mnp = makeMove (pn:ps) (gd pn p) row col

-- nextPos :: Dsf -> Path Dsf
-- nextPos path@(p:ps) = 
--     -- |np /= p = {-trace (show np)-} Path (np:path)
--     -- |otherwise = {-trace "back"-} return $ back path
--     case mnp of
--         (Just np) -> {-trace (show np)-} Path (np:path)
--         Nothing -> {-trace "back"-} return $ back path
--     where
--         mnp = makeMove path 0

nextPos :: BoardPath -> Path BoardPath
nextPos bp =
    -- |np /= p = {-trace (show np)-} Path (np:path)
    -- |otherwise = {-trace "back"-} return $ back path
    case mnp of
        (Just np) -> {-trace (show np)-} Path (BoardPath row col (np:path))
        Nothing -> {-trace "back"-} return $ BoardPath row col (back path row col)
    where
        mnp = makeMove path 0 row col
        row = getRow bp
        col = getCol bp
        path@(p:ps) = getPath bp

-- tryMany :: Int -> Pos -> Path Dsf
-- tryMany x start = return [start] >>= foldr (<=<) return (replicate x nextPos)
