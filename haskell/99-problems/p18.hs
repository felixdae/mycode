import LP17

slice l s e = f (mySplit l e) s
    where 
        f (h, t) s = g (mySplit h (s-1))
        g (hh, tt) = tt

main = do
    print $ slice ['a','b','c','d','e','f','g','h','i','k'] 3 7
