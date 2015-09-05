import Mt

countNode (Node _ sons) = foldr (\x acc -> (countNode x) + acc) 1 sons
-- countNode _ = 0

main = do
    print $ countNode tree2
    print $ countNode tree5
