import Bt

countNoLeaves EmptyTree = []
countNoLeaves (Node _ EmptyTree EmptyTree) = []
countNoLeaves (Node x l r) = (countNoLeaves l) ++ [x] ++ (countNoLeaves r)

main = do
    print $ countNoLeaves (Node 1 (Node 3 EmptyTree (Node 4 EmptyTree EmptyTree))
                     (Node 2 EmptyTree EmptyTree))
