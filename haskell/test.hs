main :: IO String
main = do
    x <- getLine
    print x
    mapM print [1,2,3]
    return x

calcBmis :: [(Double, Double)] -> [Double]
calcBmis xs = [bmi w h | (w,h) <- xs]
    where bmi weight height = weight / height^2
