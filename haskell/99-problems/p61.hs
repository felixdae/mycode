import Bt

countLeaves EmptyTree = 0
countLeaves (Node _ EmptyTree EmptyTree) = 1
countLeaves (Node _ l r) = (countLeaves l) + (countLeaves r)

main = do
    print $ countLeaves (Node 1 (Node 2 EmptyTree (Node 4 EmptyTree EmptyTree))
                     (Node 2 EmptyTree EmptyTree))
