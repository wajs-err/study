#!/bin/bash

for i in *cpp; do
	mkdir "${i%.*}"
	mv "$i" "${i%.*}"
done
