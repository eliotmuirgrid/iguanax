#
#To update the ca-bundle.crt file run make clean; make and commit the new ca-bundle.crt file please.
#
all: ca-bundle.crt

ca-bundle.crt:
ifdef OS # Windows
	perl ../libcurl/curl/scripts/mk-ca-bundle.pl
else
	../libcurl/curl/scripts/mk-ca-bundle.pl
endif

clean::
	rm ca-bundle.crt
