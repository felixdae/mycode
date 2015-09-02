import Bt

layout root = makeLayout 1 root 0

makeLayout _ EmptyTree _ = (EmptyTree, 0)
makeLayout depth (Node x l r) numSoFar = (newRoot, totalNum)
    where 
    newRoot = Node (x, (numSoFar+(numOfRes resL)+1, depth)) (rootOfRes resL) (rootOfRes resR)
    totalNum = (numOfRes resL)+(numOfRes resR)+1
    resL = makeLayout (depth+1) l numSoFar
    resR = makeLayout (depth+1) r (numSoFar+(numOfRes resL)+1)
    rootOfRes (node,num) = node
    numOfRes (node,num) = num


main = do
    print $ layout (Node 'n'
            (Node 'k'
             (Node 'c'
              (Node 'a' EmptyTree EmptyTree)
              (Node 'h'
               (Node 'g'
                (Node 'e' EmptyTree EmptyTree)
                EmptyTree
               )
               EmptyTree
              )
             )
             (Node 'm' EmptyTree EmptyTree)
            )
            (Node 'u'
             (Node 'p'
              EmptyTree
              (Node 's'
               (Node 'q' EmptyTree EmptyTree)
               EmptyTree
              )
             )
             EmptyTree
            )
            )
