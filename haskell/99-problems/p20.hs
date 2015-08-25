import LP17

removeAt n l = f $ mySplit l (n-1)
    where
    f (h, (x:xs)) = (x, h++xs)

main = do
    print $ removeAt 2 "abcd"
