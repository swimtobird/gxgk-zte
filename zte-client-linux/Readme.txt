for Linux:
just make.

for openwrt:
you should have your cross ploatform compiler and create corresponding link file

if platfrom is rg100a
cc is openwrt-gcc-rg100a
cxx is openwrt-g++-rg100a
and execute "make PF=rg100a"

if platform is hg255d
cc is openwrt-gcc-hg255d
cxx is openwrt-g++-hg255d
and execute "make PF=hg255d"

by default
cc is gcc
cxx is g++

