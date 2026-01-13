#!/usr/bin/env python3

import sys
import re
import statistics
from collections import defaultdict

time_comp = defaultdict(list)
time_all = defaultdict(list)


cur = 'none'

for line in sys.stdin:
    if "Time computation" in line:
        m = re.search(r"Time computation.* ([0-9.Ee+-]+) ", line)
        if m:
            time_comp[cur].append(float(m.group(1)))
    elif "Time all" in line:
        m = re.search(r"Time all.* ([0-9.Ee+-]+) ", line)
        if m:
            time_all[cur].append(float(m.group(1)))
    else:
        cur = line[:-1]


for k, v in time_comp.items():
    print(k)
    print("Median Time computation:", statistics.median(v))
for k, v in time_all.items():
    print(k)
    print("Median Time all:", statistics.median(v))

