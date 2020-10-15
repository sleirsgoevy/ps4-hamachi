all: IV0000-BREW00179_00-LOGMEINHAMACHI00.pkg

clean:
	rm -f IV0000-BREW00179_00-LOGMEINHAMACHI00.pkg sce_sys/icon0.png

sce_sys/icon0.png:
	wget https://vpn.net/Content/Images/hamachi-small.png -O sce_sys/icon0.png

IV0000-BREW00179_00-LOGMEINHAMACHI00.pkg: pkg.gp4 ../app/eboot.bin ../blobs/hamachid ../blobs/cpp.elf sce_sys/icon0.png
	PkgTool.Core pkg_build pkg.gp4 .
