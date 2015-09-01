import Bt

addNode x EmptyTree = Node x EmptyTree EmptyTree
addNode x (Node y l r)
    |x < y = Node y (addNode x l) r
    |otherwise = Node y l (addNode x r)

conBsTree [] tree = tree
conBsTree (x:xs) tree = conBsTree xs (addNode x tree)

main = do
    print $ conBsTree [3, 2, 5, 7, 1] EmptyTree
    print $ conBsTree [5, 3, 18, 1, 4, 12, 21] EmptyTree
