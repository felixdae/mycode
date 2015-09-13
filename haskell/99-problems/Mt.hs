module Mt
(Mtree(..)
,tree1
,tree2
,tree3
,tree4
,tree5)

where

data Mtree a = Node a [Mtree a] deriving Show

tree1 = Node 'a' []

tree2 = Node 'a' [Node 'b' []]

tree3 = Node 'a' [Node 'b' [Node 'c' []]]

tree4 = Node 'b' [Node 'd' [], Node 'e' []]

tree5 = Node 'a' [
    Node 'f' [Node 'g' []],
    Node 'c' [],
    Node 'b' [Node 'd' [], Node 'e' []]
    ]
