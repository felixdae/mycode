import System.Environment
import Text.Parsec
import Text.Parsec.String
import Control.Monad
import Data.List
import qualified Data.Set as S
import Debug.Trace (trace)

regularParse :: Parser a -> String -> Either ParseError a
regularParse p = parse p ""
 
whitespace :: Parser () 
whitespace = void $ many $ oneOf " \n\t"
lexeme p = do
    x <- p
    whitespace
    return x

nonEmptyListOfInt::Parser [Int]
nonEmptyListOfInt = do
    void $ whitespace
    lexeme $ char '['
    first <- lexeme $ num
    rest <- lexeme $ many $ (void $ lexeme (char ',')) >> lexeme num
    -- rest <- lexeme $ many (do 
    --                         void $ lexeme $ (char ',')
    --                         i <- (lexeme $ num)
    --                         return i
    --                         )
    lexeme $ char ']'
    return $ first:rest
    
emptyList = do
    void $ whitespace
    lexeme $ char '['
    lexeme $ char ']'
    return []

num :: Parser Int
num = do
    first <- satisfy (`elem` "123456789")
    rest <- many digit
    return $ (read (first:rest)::Int)

nonEmptyListOfListOfInt = do
    void $ whitespace
    lexeme $ char '['
    first <- lexeme $ listOfInt
    -- rest <- lexeme $ many $ (do
    --                             void $ lexeme $ (char ',')
    --                             li <- lexeme $ listOfInt
    --                             return li
    --                             )
    rest <- lexeme $ many $ (void $ lexeme (char ',')) >> lexeme listOfInt
    lexeme $ char ']'
    return $ first:rest

listOfListOfInt = do
    lli <- lexeme $ (try emptyList <|> nonEmptyListOfListOfInt)
    return lli

listOfInt = do
    li <- lexeme $ (try emptyList <|> nonEmptyListOfInt)
    return li
    
parseListOfList parser str = case regularParse parser str of
    Left _ -> []
    Right l -> l

makePsb 1 s = [[s+1]]
makePsb n s = concatMap (\x -> [(x+1):y | y <- makePsb (n-1) (s-x)]) [0..s] -- x+1 add back space

genRcPsbLayout _ [] = []
genRcPsbLayout s ln
    |(sum ln) + (length ln) - 1 > s = []
    |otherwise = map (makeAllRcLayout ln) (makePsb (length ln + 1) (s - ((sum ln) + (length ln) - 1)))
        where
        makeAllRcLayout ln psb = zip [0..] (tail (concatMap (\ (z, o) -> replicate z 0 ++ replicate o 1) (zip psb ln)) ++ replicate (last psb - 1) 0)

checkLayout rcl aPsb = any (\psb -> all (`elem` psb) rcl) aPsb

collectRow r layout = map (\((_,y), n) -> (y,n)) (filter (\((x,_), _) -> x==r) layout)
collectCol c layout = map (\((x,_), n) -> (x,n)) (filter (\((_,y), _) -> y==c) layout)

cst l = map sum (filter (\x -> head x == 1) (group (map (\(_, x) -> x) l)))
testGenRcPsbLayout ori s res = map (\x -> cst x == ori && s == length x && tpos x) res
tpos l = map (\(x,_)->x) l == [0..length l - 1]

getDetPoint allRowPsb allColPsb = S.toList (S.fromList (getDetPointByRow allRowPsb ++ getDetPointByCol allColPsb))
    where
    getDetPointByRow arp = foldr (\ (ri, rp) acc -> filter (\ (_,k) -> k /= 2) 
        (map (\ (y,n) -> ((ri,y),n)) 
            (foldr (\x acc -> zipWith (\ (t, a) (_, b) -> if a==b then (t, a) else (t,2)) x acc) (head rp) rp)) ++ acc) [] (zip [0..] arp)
    getDetPointByCol acp = foldr (\ (ci, cp) acc -> filter (\ (_,k) -> k /= 2) 
        (map (\ (x,n) -> ((x,ci),n)) 
            (foldr (\x acc -> zipWith (\ (t, a) (_, b) -> if a==b then (t, a) else (t,2)) x acc) (head cp) cp)) ++ acc) [] (zip [0..] acp)

solveNonogram rowCons colCons = trySolve 0 (getDetPoint allRowPsb allColPsb)
    where
    rn = length rowCons
    cn = length colCons
    allRowPsb = map (\x -> genRcPsbLayout cn x) rowCons
    allColPsb = map (\x -> genRcPsbLayout rn x) colCons
    wstart n ct ls [] = n
    wstart n ct ls (x:xs)
        |sum x > ls = wstart ct (ct+1) (sum x) xs
        |otherwise = wstart n (ct+1) ls xs
    cws = 0 --trace (show maxr ++ show maxc) maxr*cn + 0 {-maxc-}
        where
        maxr = wstart 0 0 (sum (head rowCons)) rowCons
        maxc = wstart 0 0 (sum (head colCons)) colCons

    trySolve :: Int->[((Int,Int),Int)]->[[((Int,Int),Int)]]
    trySolve n layout
        |n >= rn*cn = [layout]
        |otherwise = tryPlace (mod (div (cws+n) cn) rn, mod (cws+n) cn) 0 n layout ++ tryPlace (mod (div (cws+n) cn) rn, mod (cws+n) cn) 1 n layout
        -- |otherwise = tryPlace (div n cn, mod n cn) 0 n layout ++ tryPlace (div n cn, mod n cn) 1 n layout
    tryPlace (x,y) chi n layout
        |checkLayout (collectRow x (((x,y),chi):layout)) (getRowPsb x) && checkLayout (collectCol y (((x,y),chi):layout)) (getColPsb y) = trySolve (n+1) (((x,y),chi):layout)
        |otherwise = []
    getRowPsb r = allRowPsb !! r
    getColPsb c = allColPsb !! c

-- collectRes :: [[((Int,Int),Int)]]->String
collectRes r c = concatMap ((++"\n") . collectOneRes 0 r c)

-- collectOneRes :: Int->[((Int,Int),Int)]->String
collectOneRes n r c layout
    |n >= r*c = ""
    -- |otherwise = show (head [z | ((x,y),z) <- layout, (c*x+y)==n]) ++ (if mod (n+1) c == 0 then "\n" else " ")++ collectOneRes (n+1) r c layout
    |otherwise = (if 1 == (head [z | ((x,y),z) <- layout, (c*x+y)==n]) then "|*" else "| ") ++ (if mod (n+1) c == 0 then "|\n" else "")++ collectOneRes (n+1) r c layout

main = do
    -- print $ length (makePsb 3 8)
    -- print $ genRcPsbLayout 10 [2,3,1]
    -- print $ testGenRcPsbLayout [2,3,1] 10 (genRcPsbLayout 10 [2,3,1])
    -- x <- getArgs
    -- print x
    -- "[[3],[5],[3,1],[2,1],[3,3,4],[2,2,7],[6,1,1],[4,2,2],[1,1],[3,1],[6],[2,7],[6,3,1],[1,2,2,1,1],[4,1,1,3],[4,2,2],[3,3,1],[3,3],[3],[2,1]]" "[[2],[1,2],[2,3],[2,3],[3,1,1],[2,1,1],[1,1,1,2,2],[1,1,3,1,3],[2,6,4],[3,3,9,1],[5,3,2],[3,1,2,2],[2,1,7],[3,3,2],[2,4],[2,1,2],[2,2,1],[2,2],[1],[1]]"
    -- "[[6],[3,1,3],[1,3,1,3],[3,14],[1,1,1],[1,1,2,2],[5,2,2],[5,1,1],[5,3,3,3],[8,3,3,3]]" "[[4],[4],[1,5],[3,4],[1,5],[1],[4,1],[2,2,2],[3,3],[1,1,2],[2,1,1],[1,1,2],[4,1],[1,1,2],[1,1,1],[2,1,2],[1,1,1],[3,4],[2,2,1],[4,1]]"
    -- "[[1,1],[1,1],[1,1],[1,1]]" "[[1,1],[1,1],[1,1],[1,1]]"
    -- "[[3],[2,1],[3,2],[2,2],[6],[1,5],[6],[1],[2]]" "[[1,2],[3   ,1],[1,   5],[7 , 1],[5],[3],[4], [ 3]]"
    -- "[[5,5],[3,5,3],[2,9,2],[1,2,1,2,1],[1,11,1],[4,1,4],[4,1,4],[13],[6,6],[13],[1,2,2,1],[1,11,1],[2,9,2],[3,5,3],[5,5]]" "[[5,5],[3,5,3],[2,9,2],[1,11,1],[1,1,6,2,1],[2,1,3,3],[2,1,3,3],[7,1,3],[2,1,3,3],[2,1,3,3],[1,1,6,2,1],[1,11,1],[2,9,2],[3,5,3],[5,5]]"
    [rowDes,colDes] <- getArgs
    let rd = parseListOfList listOfListOfInt rowDes
    let cd = parseListOfList listOfListOfInt colDes
    putStr $ collectRes (length rd) (length cd) (solveNonogram rd cd)
    -- print $ regularParse num "12344"
