#!/bin/sh
target=FlameGraph
if [ "$(find "$target" -mindepth 1 -print -quit 2>/dev/null)" ]; then
    if test -e "testSuite"; then
        perf record -F 999 -g ./testSuite 1000
        perf script | ./FlameGraph/stackcollapse-perf.pl > out.perf-folded
        ./FlameGraph/flamegraph.pl out.perf-folded > flamegraph.svg
    else
        echo "testSuite executable not found, build it"
    fi
else
    echo "FlameGraph is empty, clone it from github"
fi
