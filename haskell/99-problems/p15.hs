repli l n = foldr (\e acc-> (replicate n e) ++ acc) [] l

main = do
    print $ repli "abc" 3

