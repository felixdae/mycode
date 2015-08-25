import LP17
import LP23
import System.Random
-- import Debug.Trace (trace)

rndPermu :: (RandomGen g) => g->[a]->[a]
rndPermu _ [] = []
rndPermu gen l = (egr l):(rndPermu (gg l) ((gh (mySplit l ((gr l) - 1))) ++ (gt (mySplit l (gr l)))))
    where
    -- egr l = head $ trace (show (gr l)) (gt (mySplit l ((gr l) - 1)))
    egr l = head $ (gt (mySplit l ((gr l) - 1)))
    gr l = gh (randomR (1, (length l)) gen)
    gg l = gt (randomR (1, (length l)) gen)
    -- fgr (r,g) = r
    -- fgg (r,g) = g
    gh (h, t) = h
    gt (h, t) = t

main = do
    gen <- getStdGen
    print $ rndPermu gen "abcdef"
