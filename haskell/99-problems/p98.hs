import System.Environment
import Text.Parsec
import Text.Parsec.String
import Control.Monad

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
    rest <- lexeme $ many (do 
                            void $ lexeme $ (char ',')
                            i <- (lexeme $ num)
                            return i
                            )
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
    rest <- lexeme $ many $ (do
                                void $ lexeme $ (char ',')
                                li <- lexeme $ listOfInt
                                return li
                                )
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

getRowDist nc row = 

calcNonoGram nr nc fistRowDist colDes = 

solveNonoGram rowDes ColDes = calcNonoGram nr nc rowDist colDes

main = do
    -- x <- getArgs
    -- print x
    [rowDes,colDes] <- getArgs
    print $ parseListOfList listOfListOfInt rowDes
    print $ parseListOfList listOfListOfInt colDes
    -- print $ regularParse num "12344"
