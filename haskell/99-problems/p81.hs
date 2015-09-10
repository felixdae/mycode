paths s e edges = oneStep [[s]] e edges

unvisitedSuccs node edges visited = [ y | (x,y) <- edges, x == node, not (elem y visited) ]

oneStep [] e edges = []
oneStep (curt@(frt:rf):cus) e edges
    |frt == e = (curt:(oneStep cus e edges))
    |null succes = (oneStep cus e edges)
    |otherwise = (oneStep (map (\x -> (x:curt)) succes) e edges) ++ (oneStep cus e edges)
        where
        succes = unvisitedSuccs frt edges curt


main = do
    print $ paths 1 4 [(1,2),(2,3),(1,3),(3,4),(4,2),(5,6)]
    print $ paths 2 6 [(1,2),(2,3),(1,3),(3,4),(4,2),(5,6)]

