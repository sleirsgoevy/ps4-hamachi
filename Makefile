.PHONY: app blobs ldr pkg tun

all: pkg

ldr:
	cd ldr; make

tun:
	cd tun; make

blobs:
	cd blobs; make

app: tun ldr
	cd app; make

pkg: app blobs
	cd pkg; make

clean:
	cd ldr; make clean
	cd tun; make clean
	cd blobs; make clean
	cd app; make clean
	cd pkg; make clean
