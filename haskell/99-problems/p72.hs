import Mt

bottomUp (Node c sons) = concat (map (\x -> bottomUp x) sons) ++ [c]


main = do
    print $ bottomUp tree5
