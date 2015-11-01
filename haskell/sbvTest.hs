import Data.SBV

main = do
    -- rs <- prove . forAll ["x"] $ \ x -> x * 2 .== x + (x::SWord8)
    rs <- prove $ \ x -> x * 2 .== 4 + x + (x::SWord8)
    print rs
