import Test.QuickCheck
import Data.List

qsort :: Ord a => [a] -> [a]
qsort [] = []
qsort (x:xs) = qsort lhs ++ [x] ++ qsort rhs
    where
        lhs = filter (< x) xs
        rhs = filter (>= x) xs

prop_idempotent :: [Integer] -> Bool
prop_idempotent xs = qsort (qsort xs) == qsort xs

prop_sort_model xs = sort xs == qsort xs

-- options = TestOptions {
--             no_of_tests = 1000,
--             length_of_tests = 1,
--             debug_tests = True
--             }

main = do
    -- print "ffdd"
    -- quickCheck (prop_idempotent :: [Integer] -> Bool)
    quickCheck prop_idempotent
    quickCheckWith stdArgs {maxSuccess = 500} prop_idempotent
    -- verboseCheck prop_idempotent
    quickCheck (prop_sort_model :: [Integer] -> Bool)
    -- runTests "test" options [
    --     run prop_idempotent,
    --     run prop_sort_model
    --     ]
