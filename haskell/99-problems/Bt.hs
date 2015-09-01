module Bt
(Btree(..)
,makeTreeNode)
where

data Btree a = EmptyTree | Node a (Btree a) (Btree a) deriving (Show)

makeTreeNode :: a->Btree a
makeTreeNode v = Node v EmptyTree EmptyTree
