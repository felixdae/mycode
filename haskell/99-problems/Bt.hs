module Bt
(Btree(..)
,mirror
,symmetric
,cbalTree
,makeTreeNode)
where

data Btree a = EmptyTree | Node a (Btree a) (Btree a) deriving (Show)

cbalTree 0 = [EmptyTree]
-- cbalTree 1 = Node 'x' EmptyTree EmptyTree

cbalTree n
    |mod n 2 == 1 = expTree 'x' halfCbal halfCbal
    |otherwise = expTree 'x' halfCbal almostHalfCbal ++ expTree 'x' almostHalfCbal halfCbal
        where 
        almostHalfCbal = cbalTree ((div n 2)-1) 
        halfCbal = cbalTree (div n 2)
        expTree c leftL rightL = [(Node c l r) | l <- leftL, r <- rightL]

makeTreeNode :: a->Btree a
makeTreeNode v = Node v EmptyTree EmptyTree

mirror EmptyTree EmptyTree = True
mirror EmptyTree _ = False
mirror _ EmptyTree = False
mirror (Node _ la ra) (Node _ lb rb) = (mirror la rb) && (mirror ra lb)

symmetric (Node _ l r) = mirror l r
