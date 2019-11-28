import sys
import random

pre = ""
for line in sys.stdin:
  line = line.replace("\n", "")
  seg = line.split(" ")
  if (seg[0] != pre):
    print line
  pre = seg[0]
