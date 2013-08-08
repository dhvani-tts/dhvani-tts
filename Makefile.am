SUBDIRS = database man src/soundtouch4c src
#EXTRA_DIST = dhvani 

deb: dist
	if [ `whereis dpkg-buildpackage | cut -f2 -d" "` == "dpkg-buildpackage:" ] ; then echo "You have to install dpkg-buildpackage" ; exit 1 ; fi
	tar -xvzf $(PACKAGE)-$(VERSION).tar.gz
	cd $(PACKAGE)-$(VERSION) ; dpkg-buildpackage -rfakeroot
