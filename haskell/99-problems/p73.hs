import Mt

lispDisplay (Node c []) = [c] 
lispDisplay (Node c sons) = "("++[c]++concat (map (\x-> " "++(lispDisplay x)) sons)++")"


main = do
    print $ lispDisplay tree1
    print $ lispDisplay tree2
    print $ lispDisplay tree3
    print $ lispDisplay tree4
    print $ lispDisplay tree5
