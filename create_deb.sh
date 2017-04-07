#!/bin/bash

UPSTREAM_VERSION="$(dpkg-parsechangelog --show-field Version | sed 's/-.*$//')"
cd ..
tar zcvf libexecs_${UPSTREAM_VERSION}.orig.tar.gz --transform "s/^s2argv-execs/libexecs_${UPSTREAM_VERSION}/" s2argv-execs
cd s2argv-execs
debuild -uc -us -sa
