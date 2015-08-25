import LP11

ff (Single c) acc = c:acc
ff (Multiple n c) acc = (replicate n c) ++ acc

decodeModified = foldr ff []

main = do
    print $ decodeModified [Multiple 4 'a',Single 'b',Multiple 2 'c',
            Multiple 2 'a',Single 'd',Multiple 4 'e']
