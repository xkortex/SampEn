sampen
======

Fork, updating, and modernizing of [Sample Entropy (SampEn)](http://www.physionet.org/physiotools/sampen/). Port of v1.2 C-language code from [PhysioNet](http://www.physionet.org/physiotools/sampen/c/) last updated 1 November 2004, by George Moody. 
Originally released Copyright (C) 2002-2004 by Doug Lake with GNU GPL v.2
I will be keeping with GPL 2 unless I decide to update. 
---

---
usage: sampen [OPTIONS ...] [TEXT-FILE]
<br> where OPTIONS may include:
<br> -h    print this usage summary
<br>  -m M  set the maximum epoch length to M (default: 2)
<br>  -n    normalize such that the mean of the input is 0 and the sample variance is 1
<br>  -r R  set the tolerance to R (default: 0.2)
<br>  -v    output an estimate of the standard deviation of each SampEn
<br> TEXT-FILE should contain the time series (a column of decimal numbers); if omitted, sampen reads its standard input.  The output contains values of SampEn(m,r,N) where m is the epoch length, r is the tolerance, and N is the length of the input series.

---
