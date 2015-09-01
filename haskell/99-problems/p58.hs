import Bt

symCbalTrees n = filter symmetric $ cbalTree n

main = do
    print $ symCbalTrees 5
