{-# LANGUAGE ImplicitParams #-}
import Regex.Genex

main = let ?maxRepeat = 13 in genexPrint ["(abc)*"]
    -- gg <- genex ["a*"]
    --genexPrint ["([^X]|XCC)*"]
    -- gg <- genex ["([^X]|XCC)*"]
    -- print gg
