=======
OpenCF
======

Here is an implementation of collaborative filtering system, which is the most popular algorithm for recommender systems.

More information can be obtained from the slider: report/OpenCF-report.pdf

OpenCF implemented both user-based CF and item-based CF, and optimizing methods like:

1. Row normalization for Rating matrix.
2. Similarity functions:
    * raw cosine
    * adjusted cosine
    * pearson correlation
3. Similarity Summing:
    * Direct summing
    * Normalized(1-order) similarity summing
    * Probability similarity summing

# Compact ID

`compact` is used to compact discontinuous user-id and item-id.

```bash
# compact data/uir to data/uir.compact, -U and -I specifies mapping file names, which is used to restore ids.
./compact -f data/uir -o data/uir.compact -U data/user.map -I data/item.map

# help
./compact -h
```

# Similarity computing

`similarity` computes similarity matrix, with various methods. 

```bash
# help
./similarity -h

# raw cosine similarity
./similarity -f data/uir.compact -o data/ii.cos

# adjusted cosine similarity
./similarity -a 1 data/uir.compact -o data/ii.acos

# pearson correlation similarity
./similarity -a 2 data/uir.compact -o data/ii.corr
```

The data format `similarity` accept is:

```
userid1 itemid1 rating1
userid2 itemid2 rating2
...
```

where `userid` and `itemid` are `int` compatible, `rating`s are `float` compatible.

# Prediction

`prediction` uses similarity-file and rating-file to compute prediction matrix.

```bash
./predict -s data/ii.cos -f data/uir.compact -o data/uip.ii.compact
```

# Restore IDs

Use `compact -r` to restore user/item ids, mapping-files should be specified.

```bash
./compact -r -f data/uip.ii.compact -o data/uip.ii -U data/user.map -I data/item.map 
```

# Tools

## Rating 

Dataset `data/train` and `data/test` are extracted from `data/t_alibaba_data.csv` manually. The format in `train` is not compatible. `rating` is used to generate compatible rating-file from data-files with this format:

```
userid1 itemid1 operation1  month1  day1
userid2 itemid2 operation2  month2  day2
...
```

```bash
# generate compatible rating-file: data/uir
rating/rating -i data/train -o data/uir -l data/dealdate.last -c data/deal.count

# help
rating/rating
```

## Post processing

`postprocess` is used to de-emphasize items that already purchased by user.

```bash
postprocess/postprocess -p data/uip.ii -l data/dealdate.last -c data/deal.count -o data/uip.ii.mod
```

## Evaluation

```bash
# prepare test-file, generates data/test.ui
evaluate/prepare_test.sh data/test

# sort and filter prediction-file data/uip.ii to data/ii.ui
evaluate/sort_prediction.sh data/uip.ii

# evaluate sorted-prediction-file data/uip.ii.ui
evaluate/evaluate -p data/uip.ii.ui -t data/test.ui -o data/ii.curve
```

