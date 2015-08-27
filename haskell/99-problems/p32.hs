-- import Debug.Trace (trace)

myGcd x y
    |x < 0 = myGcd (abs x) y
    |y < 0 = myGcd x (abs y)
    |y < x = myGcd y x
    |x == 0 = y
    -- |otherwise = trace (show x ++ ":" ++ show y) myGcd (mod y x) x
    |otherwise = myGcd (mod y x) x
   

main = do
    print $ myGcd 36 63
    print $ myGcd (-3) (-6)
    print $ myGcd (-3) 6
