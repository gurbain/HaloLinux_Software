#!/bin/bash
echo VERTIGO MAINTENANCE
echo "*** Check GPF Results directory size"
du -s -h /home/GPF_DIR/Results/*
echo
echo "*** Check GPF Results contents"
tree -L 3 /home/GPF_DIR/Results/
echo VERTIGO MAINTENANCE COMPLETE
