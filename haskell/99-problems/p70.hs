import Mt

tree2String :: Mtree Char -> [Char]
tree2String (Node c sons) = [c] ++ (concatMap (++"") (map (\x -> tree2String x) sons)) ++ "^"

-- data DMt a = OpenList|Mtree a

string2Tree :: [Char]->[Maybe (Mtree Char)]->Mtree Char
string2Tree [] [Just x@(Node _ _)] = x
string2Tree (x:xs) stk
    |x /= '^' = string2Tree xs (Nothing:(Just (Node x [])):stk)
    -- |x /= '^' = string2Tree xs (OpenList:(Mtree x):stk)
    |otherwise = string2Tree xs (traceBack stk [])
        where
        -- traceBack :: [Maybe (Mtree Char)]->[Maybe (Mtree Char)]->[Maybe (Mtree Char)]
        traceBack (Nothing:(Just (Node cc _)):ss) ts = (Just (Node cc ts)):ss
        traceBack (Just tn@(Node _ _):ss) ts = traceBack ss (tn:ts)

main = do
    print $ tree2String tree5
    print $ string2Tree (tree2String tree5) []
