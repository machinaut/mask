#!/bin/bash

PDF="WC37-Badger-Wintercroft.pdf"

for i in $(seq 6 15)
do
  pdf2svg $PDF out$i.svg $i
done
