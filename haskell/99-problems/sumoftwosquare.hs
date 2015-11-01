import Nt
import Debug.Trace (trace)

squareOfOne :: Integer->Integer
squareOfOne n = trySquare1 n 1
    where
    trySquare1 t f
        |t == f*f = f
        |t < f*f = 0
        |otherwise = trySquare1 t (f+1)

squareOfTwo :: Integer->(Integer, Integer)
squareOfTwo n = trace (show n) trySquare2 n 1
    where
    trySquare2 t f
        |squareOfOne (t - f*f) /= 0 = (f, squareOfOne (t - f*f))
        |otherwise = trySquare2 n (f+1)


main = do
    -- print $ takeWhile (\x -> mod x 2 == 1) (takeWhile (<200) primesTME)
    mapM_ print (map squareOfTwo (filter (\x -> mod x 4 == 1) (takeWhile (<200) primesTME)))
