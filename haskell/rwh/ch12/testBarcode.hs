import System.IO
import System.Environment
import qualified Data.ByteString.Lazy as BL
-- import Data.List
import Debug.Trace (trace)

import Parse
import Barcode

main = do
    -- print (checkDigit [9,7,8,0,1,3,2,1,1,4,6,7])
    -- print leftOddList
    -- print leftEvenList
    -- print rightList
    -- print leftOddCodes
    -- print leftEvenCodes
    -- print rightCodes
    -- print parityCodes
    -- print testLOL
    -- print testPL
    -- print $ encodeEAN13 "978755026327"
    -- let bits = [0,0,1,1,0,0,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0]
    -- print $ runLength bits
    -- print $ runLengths bits
    -- let group = scaleToOne [2,6,4,4]
    -- print $ distance group (head leftOddSRL)
    -- print $ distance group (head leftEvenSRL)
    -- print $ bestScores leftOddSRL [2,6,4,4]

    let input = [(2,Zero),(2,One),(2,Zero),(2,One),(6,Zero),(2,One),(4,Zero),(6,One),(2,Zero),(4,One),(2,Zero),(2,One),(2,Zero),(4,One),(6,Zero),(4,One),(4,Zero),(4,One),(2,Zero),(2,One),(2,Zero),(8,One),(2,Zero),(4,One),(2,Zero),(4,One),(4,Zero),(2,One),(2,Zero),(2,One),(2,Zero),(2,One),(4,Zero),(4,One),(4,Zero),(2,One),(4,Zero),(4,One),(4,Zero),(2,One),(2,Zero),(2,One),(6,Zero),(4,One),(2,Zero),(2,One),(2,Zero),(8,One),(2,Zero),(6,One),(2,Zero),(4,One),(2,Zero),(6,One),(2,Zero),(4,One),(2,Zero),(2,One),(2,Zero)]
    print $ candidateDigits $ input
    exeName <- getProgName
    args <- getArgs
    -- doArgs exeName args
    case args of 
        [] -> print $ "usage: " ++ exeName ++ " filename"
        (fileName:_) -> withFile fileName ReadMode (
            \h -> do
                contents <- BL.hGetContents h
                case (parse parseRawPPM contents) of 
                    Left err -> print $ "error: " ++ err
                    Right pixmap -> {-trace (show pixmap)-} print $ findEAN13 pixmap
            )
