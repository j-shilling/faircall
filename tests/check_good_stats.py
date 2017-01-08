#!/usr/bin/python3

import sys
import os
import subprocess
import statistics

def progress(count, total, status=''):
    bar_len = 60
    filled_len = int(round(bar_len * count / float(total)))

    percents = round(100.0 * count / float(total), 1)
    bar = '=' * filled_len + '-' * (bar_len - filled_len)

    sys.stdout.write('[%s] %s%s ...%s\r' % (bar, percents, '%', status))
    sys.stdout.flush() 

nstudents = 30
ncalls = 100
ntests = 1000

todo = (nstudents + ncalls) * ntests
done = 0

cmd = sys.argv[1]
filename = "roster"

count = 0
calls = []
while (count < ntests):
    i = 0
    while (i < nstudents):
        progress(done, todo)
        subprocess.Popen ([cmd, '-d', filename, '-a', str(i), 'class']).wait()
        i += 1
        done += 1

    i = 0
    while (i < ncalls):
        subprocess.Popen ([cmd, '-d', filename, '-1', 'class'], \
                stdout=subprocess.PIPE).wait()
        progress(done, todo)
        i += 1
        done += 1

    info = subprocess.Popen ([cmd, '-d', filename, '-m', '-i', 'class'],    \
            stdout=subprocess.PIPE)                                         \
            .communicate()[0]                                               \
            .decode(encoding="UTF-8")

    os.remove(filename)

    for line in info.split('\n'):
        if ':' in line:
            calls.append(int(line.split(':')[1]))

    count += 1

calls.sort()

median = statistics.median(calls)
mid = int(len(calls) / 2)

if (len(calls) % 2 == 0):
    q1 = statistics.median(calls[:mid])
    q3 = statistics.median(calls[mid:])
else:
    q1 = statistics.median(calls[:mid])
    q3 = statistics.median(calls[mid+1:])

iqr = q3 - q1
lower = q1 - (iqr * 1.5)
upper = q3 + (iqr * 1.5)

print ("\n")
print ("Data points:", len (calls))
print ("Coefficient of Variation:", \
        statistics.stdev (calls) / statistics.mean (calls))

outliers = []
for val in calls:
    if (val < lower):
        outliers.append(val)

    if (val > upper):
        outliers.append(val)

print ("Outliers found:", len(outliers))
