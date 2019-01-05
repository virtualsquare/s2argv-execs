#!/bin/bash

if [ $1 == 'newtag' ]
then
	UPSTREAM_VERSION="$(dpkg-parsechangelog --show-field Version | sed 's/-.*$//')"
	cd ..
	tar zcvf libexecs_${UPSTREAM_VERSION}.orig.tar.gz --transform "s/^s2argv-execs/libexecs_${UPSTREAM_VERSION}/" --exclude "s2argv-execs/debian" --exclude "s2argv-execs/.git" s2argv-execs
	cd s2argv-execs
fi

debuild -uc -us -sa
