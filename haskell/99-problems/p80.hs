data Graph a = Graph [a] [(a,a)] deriving Show
data Adj a = Adj [(a,[a])] deriving Show

graphToAdj (Graph [] _) = Adj []
graphToAdj (Graph (x:xs) edges) = pushAdj (x, getAdjs x edges) (graphToAdj (Graph xs edges))
    where
    pushAdj adj (Adj l) = Adj (adj:l)
    getAdjs _ [] = []
    getAdjs node ((s,e):xedges)
        |s == node = e:(getAdjs node xedges)
        |e == node = s:(getAdjs node xedges)
        |otherwise = getAdjs node xedges


main = do
    print $ graphToAdj (Graph ['b','c','d','f','g','h','k'] [('b','c'),('b','f'),('c','f'),('f','k'),('g','h')])

