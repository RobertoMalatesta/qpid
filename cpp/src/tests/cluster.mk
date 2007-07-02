if CLUSTER
# Cluster tests makefile fragment, to be included in Makefile.am
# 

lib_cluster = $(abs_builddir)/../libqpidcluster.la

# NOTE: Programs using the openais library must be run with gid=ais
# You should do "newgrp ais" before running the tests to run these.
# 

#
# Cluster tests.
# 

TESTS+=ais_check
EXTRA_DIST+=ais_check

TESTS+=Cpg
check_PROGRAMS+=Cpg
Cpg_SOURCES=Cpg.cpp
Cpg_LDADD=$(lib_cluster) -lboost_unit_test_framework

TESTS+=Cluster
check_PROGRAMS+=Cluster
Cluster_SOURCES=Cluster.cpp Cluster.h
Cluster_LDADD=$(lib_cluster) -lboost_unit_test_framework

check_PROGRAMS+=Cluster_child 
Cluster_child_SOURCES=Cluster_child.cpp Cluster.h
Cluster_child_LDADD=$(lib_cluster) -lboost_test_exec_monitor

endif
