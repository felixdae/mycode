import Bt

collectLeaves EmptyTree = []
collectLeaves (Node x EmptyTree EmptyTree) = [x]
collectLeaves (Node x l r) = (collectLeaves l) ++ (collectLeaves r)

main = do
    print $ collectLeaves (Node 1 (Node 2 EmptyTree (Node 4 EmptyTree EmptyTree))
                     (Node 2 EmptyTree EmptyTree))
