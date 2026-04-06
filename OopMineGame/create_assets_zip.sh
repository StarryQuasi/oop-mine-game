#!/bin/bash

rm -f assets.zip &&
cd assets &&
zip -r ../assets.zip * &&
cd ..
