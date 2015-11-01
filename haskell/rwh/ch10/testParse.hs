import Parse
import System.IO
import qualified Data.ByteString.Lazy as BL


main = do
    withFile "./bird.pgm" ReadMode (
        \h -> do
            contents <- BL.hGetContents h
            print $ parse parseRawPGM contents
        )
        -- print contents)
        -- BL.putStr contents)
