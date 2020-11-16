import matplotlib.pyplot as plt
from pylab import *

datahits=loadtxt("hits_misses.txt", dtype=np.int32)
data=loadtxt("access_time.txt", dtype=np.int32)

x=1
y=1
z=0

tot_access_time_cache_hit = 0
tot_access_time_cache_miss = 0

for i in range(0,8):
	if datahits[i]>100:
		plt.plot(range(x,x+datahits[i]),data[z:z+datahits[i]])
		x= x + datahits[i]
		for j in range(0,datahits[i]):
			tot_access_time_cache_hit = tot_access_time_cache_hit + data[z+j];

	if datahits[i]<100:
		plt.plot(range(y,y+datahits[i]),data[z:z+datahits[i]])
		y= y + datahits[i]
		for j in range(0,datahits[i]):
			tot_access_time_cache_miss = tot_access_time_cache_miss + data[z+j];		

	z= z + datahits[i]

avg_access_time_cache_hit = tot_access_time_cache_hit / x;
avg_access_time_cache_miss = tot_access_time_cache_miss / y;

print 'Average access time for Cache Hit is', avg_access_time_cache_hit
print 'Average access time for Cache Miss is', avg_access_time_cache_miss
print 'Open access_times.png in this folder to view the plots.'

savefig("access_times.png")



