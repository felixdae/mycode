import LP17
-- import Debug.Trace (trace)

rotate l n
    -- |n < 0 = trace ((show l) ++ (show n)) protate l ((length l) + n)
    |n < 0 = protate l ((length l) + n)
    |otherwise = protate l n
        where
        -- protate ll nn = trace ((show ll) ++ (show nn)) f $ mySplit ll nn
        protate ll nn = f $ mySplit ll nn
        f (h, t) = t++h

main = do
    print $ rotate ['a','b','c','d','e','f','g','h'] 3
    print $ rotate ['a','b','c','d','e','f','g','h'] (-2)
