cd ../src && make && cp -f AC ../benchmark && cd ../benchmark

./AC -l 1 -lr 0.3 -hs 16 ../benchmark/AminoAcidsCorpus/EP
./AC -l 1 -lr 0.2 -hs 16 ../benchmark/AminoAcidsCorpus/XV
./AC -l 2 -lr 0.02 -hs 16 ../benchmark/AminoAcidsCorpus/FV
./AC -l 7 -lr 0.6 -hs 40 ../benchmark/AminoAcidsCorpus/FM
./AC -l 2 -lr 0.09 -hs 24 ../benchmark/AminoAcidsCorpus/HA
./AC -l 5 -lr 0.4 -hs 40 ../benchmark/AminoAcidsCorpus/AP
./AC -l 4 -lr 0.3 -hs 40 ../benchmark/AminoAcidsCorpus/DA
./AC -l 4 -lr 0.4 -hs 40 ../benchmark/AminoAcidsCorpus/MJ
./AC -l 4 -lr 0.4 -hs 40 ../benchmark/AminoAcidsCorpus/HI
./AC -l 5 -lr 0.4 -hs 40 ../benchmark/AminoAcidsCorpus/SA
./AC -l 5 -lr 0.4 -hs 40 ../benchmark/AminoAcidsCorpus/LC
./AC -l 5 -lr 0.4 -hs 40 ../benchmark/AminoAcidsCorpus/EC
./AC -l 7 -lr 0.17 -hs 40 ../benchmark/AminoAcidsCorpus/HT
./AC -l 6 -lr 0.17 -hs 40 ../benchmark/AminoAcidsCorpus/SC
./AC -l 7 -lr 0.17 -hs 40 ../benchmark/AminoAcidsCorpus/HS
./AC -l 7 -lr 0.17 -hs 40 ../benchmark/AminoAcidsCorpus/BT
