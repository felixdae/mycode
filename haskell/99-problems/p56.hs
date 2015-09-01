import Bt

main = do
    print $ symmetric (Node 'x' (Node 'x' EmptyTree EmptyTree) EmptyTree)
    print $ symmetric (Node 'x' (Node 'x' EmptyTree EmptyTree) (Node 'x' EmptyTree EmptyTree))
    print $ symmetric (Node 5 (Node 3 (Node 1 EmptyTree EmptyTree) (Node 4 EmptyTree EmptyTree)) (Node 18 (Node 12 EmptyTree EmptyTree) (Node 21 EmptyTree EmptyTree)))
