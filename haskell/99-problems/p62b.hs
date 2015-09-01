import Bt

atLevel EmptyTree _ = []
-- atLevel (Node _ EmptyTree EmptyTree) = []
atLevel (Node x _ _) 1 = [x]
atLevel (Node _ l r) n = (atLevel l (n-1)) ++ (atLevel r (n-1))

main = do
    print $ atLevel (Node 1 (Node 3 EmptyTree (Node 4 EmptyTree EmptyTree))
                     (Node 2 EmptyTree EmptyTree)) 3
    print $ atLevel (Node 1 (Node 3 EmptyTree (Node 4 EmptyTree EmptyTree))
                     (Node 2 EmptyTree EmptyTree)) 2
    print $ atLevel (Node 1 (Node 3 EmptyTree (Node 4 EmptyTree EmptyTree))
                     (Node 2 EmptyTree EmptyTree)) 1
