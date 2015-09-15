import System.Environment
import Text.Parsec
import Text.Parsec.String
import Control.Monad
import Data.List

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

makePsb 1 s = [[s]]
makePsb n s = concatMap (\x -> [x:y | y <- makePsb (n-1) (s-x)]) [0..s]

genRcPsbLayout _ [] = []
genRcPsbLayout s ln
    |(sum ln) + (length ln) - 1 > s = []
    |otherwise = map (makeAllRcLayout 0 ln) (makePsb (length ln + 1) (s - ((sum ln) + (length ln) - 1)))
        where
        makeAllRcLayout _ [] [] = []
        makeAllRcLayout start [ys] [ns] = (map (\x -> (start + x, 1)) [0..(ys-1)]) ++ makeAllRcLayout (start + ys) [] [ns]
        makeAllRcLayout start ln psb
            |length psb > length ln = (map (\x -> (start + x, 0)) [0..(head psb-1)]) ++ makeAllRcLayout (start + (head psb)) ln (tail psb)
            |otherwise = (map (\x -> (start + x, 1)) [0..(head ln-1)]) ++ [((start + head ln),0)] ++ makeAllRcLayout (start + 1 + (head ln)) (tail ln) psb


checkLayout rcl [] = False
checkLayout rcl (psb:rcPsb)
    |harmony rcl psb = True
    |otherwise = checkLayout rcl rcPsb
        where
        harmony [] p = True
        harmony (x:xs) p
            |elem x p = harmony xs p
            |otherwise = False
        -- harmony l p = foldr (`elem` p) True l

collectRow r layout = map (\((_,y), n) -> (y,n)) (filter (\((x,_), _) -> x==r) layout)
collectCol c layout = map (\((x,_), n) -> (x,n)) (filter (\((_,y), _) -> y==c) layout)

cst l = map sum (filter (\x -> head x == 1) (group (map (\(_, x) -> x) l)))
testGenRcPsbLayout ori s res = map (\x -> cst x == ori && s == length x && tpos x) res
tpos l = map (\(x,_)->x) l == [0..length l - 1]

solveNonogram rowCons colCons = trySolve 0 []
    where
    rn = length rowCons
    cn = length colCons
    allRowPsb = map (\x -> genRcPsbLayout cn x) rowCons
    allColPsb = map (\x -> genRcPsbLayout rn x) colCons
    trySolve n layout
        |n >= rn*cn = [layout]
        |otherwise = tryPlace (div n cn, mod n cn) 0 n layout ++ tryPlace (div n cn, mod n cn) 1 n layout
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
    |otherwise = show (head [z | ((x,y),z) <- layout, (c*x+y)==n]) ++ (if mod (n+1) c == 0 then "\n" else " ")++ collectOneRes (n+1) r c layout

main = do
    -- print $ length (makePsb 3 8)
    -- print $ genRcPsbLayout 10 [2,3,1]
    -- print $ testGenRcPsbLayout [2,3,1] 10 (genRcPsbLayout 10 [2,3,1])
    -- x <- getArgs
    -- print x
    [rowDes,colDes] <- getArgs
    let rd = parseListOfList listOfListOfInt rowDes
    let cd = parseListOfList listOfListOfInt colDes
    putStr $ collectRes (length rd) (length cd) (solveNonogram rd cd)
    -- print $ regularParse num "12344"
