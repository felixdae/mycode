dupli = foldr (\e acc-> e:e:acc) []

main = do
    print $ dupli [1,2,3]
