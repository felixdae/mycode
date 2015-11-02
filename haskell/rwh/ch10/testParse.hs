import Parse
import System.IO
import qualified Data.ByteString.Lazy as BL
import System.Environment

main = do
    exeName <- getProgName
    args <- getArgs
    -- doArgs exeName args
    case args of 
        [] -> print $ "usage: " ++ exeName ++ " filename"
        (fileName:_) -> withFile fileName ReadMode (
            \h -> do
                contents <- BL.hGetContents h
                print $ parse parseRawPGM contents
            )

        -- where
        -- doArgs _ (fileName:_) =
        --     withFile fileName ReadMode (
        --         \h -> do
        --             contents <- BL.hGetContents h
        --             print $ parse parseRawPGM contents
        --         )
        -- doArgs exeName _ = print $ "usage: " ++ exeName ++ " filename"
        -- print contents)
        -- BL.putStr contents)
