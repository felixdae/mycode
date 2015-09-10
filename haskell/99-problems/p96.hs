import System.Environment
import Text.Parsec
import Text.Parsec.String
import Debug.Trace (trace)

regularParse :: Parser a -> String -> Either ParseError a
regularParse p = parse p ""

doParse parser str = case regularParse parser str of
    Left _ -> False
    _ -> True

anStr = many1 alphaNum

hyphenLead = do
    hypen <- char '-'
    rest <- anStr
    return $ (hypen:rest)

withHypen = do
    fs <- many anStr
    rest <- many1 hyphenLead 
    return $ trace (show rest) concat (fs++rest)

identifier = do
    fc <- letter
    rest <- (try (anStr <* eof) <|> (withHypen <* eof))
    -- rest <- withHypen
    return $ (fc:rest)
    --regularParse (many1 letter) str

main = do
    (arg1:_) <- getArgs
    print arg1
    print $ doParse identifier arg1
