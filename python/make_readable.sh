#!/bin/bash

sed -i 's/\[//g'                           directions/*
sed -i 's/\]//g'                           directions/*
sed -i 's/)//g'                            directions/*
sed -i 's/(//g'                            directions/*
sed -i 's/), (//g'                         directions/*
sed -i 's/\([0-9]\), \([0-9-]\)/\1 \2/g'   directions/*

