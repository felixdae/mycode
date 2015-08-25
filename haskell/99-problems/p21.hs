import LP17

insertAt c l n = f c $ mySplit l (n-1)
    where
    f c (h, t) = h ++ (c:t)

main = do
    print $ insertAt 'X' "abcd" 2
