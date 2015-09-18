main :: IO () --String
main = do
    -- x <- getLine
    -- print x
    -- mapM print [1,2,3]
    print $ take 16 fibs
    -- return x

fibs = 1:1:[ x+y | x <- fibs 
           | y <- tail fibs ]

calcBmis :: [(Double, Double)] -> [Double]
calcBmis xs = [bmi w h | (w,h) <- xs]
    where bmi weight height = weight / height^2
