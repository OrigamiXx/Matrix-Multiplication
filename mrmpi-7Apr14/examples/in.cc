# OINK script for connected component finding

variable t equal time
variable p equal nprocs

set scratch SCRATCH
#set verbosity 1
#set timer 1

rmat 16 2 0.25 0.25 0.25 0.25 0.0 12345 -o NULL mre
edge_upper -i mre -o NULL mre
cc_find 0 -i mre -o tmp.cc mrc
print "CC: $t secs on $p procs"
cc_stats -i mrc
