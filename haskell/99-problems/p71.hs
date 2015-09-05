import Mt

internalPath (Node _ sons) depth = foldr (\e acc -> acc + internalPath e (depth+1)) depth sons

main = do
    print $ internalPath tree4 0
    print $ internalPath tree5 0
