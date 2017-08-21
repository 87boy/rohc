# ROHC Library

## [System requirements for building the ROHC library from sources](https://rohc-lib.org/wiki/doku.php?id=library-requirements)

### System requirements for building the ROHC library from release archives

#### Ubuntu

    sudo apt-get install build-essential

#### CentOS

    sudo yum install gcc make

### System requirements for building the ROHC library from source repositories

#### Ubuntu

    sudo apt-get install git
    sudo apt-get install build-essential
    sudo apt-get install automake autoconf libtool
    # [libtool installed but not found in /usr/bin](https://superuser.com/questions/928653/libtool-installed-but-not-found-in-usr-bin)
    sudo apt-get install libtool-bin

#### CentOS

    sudo yum install git
    sudo yum install gcc make
    sudo yum install automake autoconf libtool

### System requirements for building and running ROHC tests

#### Ubuntu
    
    $ sudo apt-get install libpcap-dev
    $ sudo apt-get install libcmocka-dev # for versions >= 1.7.0 only
    $ sudo apt-get install valgrind xsltproc # optional, required for running library tests within Valgrind

#### CentOS

    $ sudo yum install libpcap libpcap-devel
    $ sudo yum install valgrind libxslt # optional, required for running library tests within Valgrind

### System requirements for building and running ROHC statistics

#### Ubuntu

    sudo apt-get install libpcap-dev coreutils gnuplot

#### CentOS

    sudo yum install libpcap libpcap-devel
    sudo yum install coreutils gnuplot

### System requirements for building API documentation

#### Ubuntu

    sudo apt-get install doxygen

#### CentOS

    sudo yum install doxygen

## Install the ROHC library and its applications

#### Ubuntu

    sudo apt-get install libpcap-dev
    sudo apt-get install gnuplot
    # Requires a Linux kernel
    
    sudo apt-get install doxygen
    # the dot binary from the graphviz project
    sudo apt-get install graphviz
    # doxy2man binary
    
    # help2man binary
    sudo apt-get install help2man
    
    # cmocka library and headers
    sudo apt-get install libcmocka-dev
    sudo apt-get install valgrind xsltproc # optional, required for running library tests within Valgrind

#### CentOS

    sudo yum install libpcap libpcap-devel
    sudo yum install gnuplot
    # a Linux kernel
    sudo yum install kernel-devel
    sudo yum install doxygen
    # the dot binary from the graphviz project
    sudo yum install graphviz
    # doxy2man binary
    
    # help2man binary
    sudo yum install help2man
    
    # cmocka library and headers

    sudo yum install valgrind libxslt # optional, required for running library tests within Valgrind
    # For compiling cmocka
    sudo yum install cmake

- [I Need the Kernel Source](https://wiki.centos.org/HowTos/I_need_the_Kernel_Source)

### Get Source

    git clone https://github.com/didier-barvaux/rohc.git
    cd rohc/
    git branch -a
    git tag -l
    git checkout rohc-2.0.0
    
    wget https://rohc-lib.org/download/rohc-2.0.x/2.0.0/rohc-2.0.0.tar.xz --no-check-certificate
    wget https://rohc-lib.org/download/rohc-2.0.x/2.0.0/rohc-2.0.0.tar.xz.asc --no-check-certificate
    wget https://rohc-lib.org/download/rohc-2.0.x/2.0.0/rohc-2.0.0.tar.xz.sha256 --no-check-certificate
    tar xvJf rohc-2.0.0.tar.xz

Get libpcap source code 

    git clone https://github.com/the-tcpdump-group/libpcap.git
    cd libpcap/
    git branch -a
    git tag -l
    git checkout libpcap-1.8.1
    
    wget https://github.com/the-tcpdump-group/libpcap/archive/libpcap-1.8.1.tar.gz --no-check-certificate
    tar zxvf libpcap-1.8.1.tar.gz

Get cmocka source code

    git clone git://git.cryptomilk.org/projects/cmocka.git
    git clone https://github.com/clibs/cmocka.git
    cd cmocka/
    git branch -a
    git tag -l
    git checkout cmocka-1.1.1
    
    wget https://cmocka.org/files/1.1/cmocka-1.1.1.tar.xz --no-check-certificate
    wget https://cmocka.org/files/1.1/cmocka-1.1.1.tar.xz.asc --no-check-certificate
    tar xvJf cmocka-1.1.1.tar.xz
    
    cd cmocka/build/
    cmake ./../ -DCMAKE_INSTALL_PREFIX=/usr/local/ -DCMAKE_BUILD_TYPE=Release
    cmake ~/git/cmocka/ -DCMAKE_INSTALL_PREFIX=/usr/local/ -DCMAKE_BUILD_TYPE=Release

make ROHC

    $ cd rohc/
    $ ./autogen.sh --prefix=/usr/local
    # ./configure: line 15200: syntax error near unexpected token `CMOCKA,'
    # ./configure: line 15200: ` PKG_CHECK_MODULES(CMOCKA, cmocka >= 0.3.1,'
    $ sudo apt-get install pkg-config # Ubuntu
    $ sudo yum install pkgconfig      # CentOS
    # ERROR: cmocka library/headers not found
    [pkg-config on CentOS](https://hamiltonwongcpp.wordpress.com/2015/11/28/pkg-config-on-centos/)
    sudo find / -iname cmocka.pc
    sudo cp /usr/lib/pkgconfig/cmocka.pc /usr/lib64/pkgconfig/
    
    # error while loading shared libraries cannot open shared object file no such file or directory
    $ sudo ldconfig
    $ ll /etc/ld.so.conf
    $ cat /etc/ld.so.conf
    include ld.so.conf.d/*.conf
    $ ll /etc/ld.so.conf.d/
    $ su root
    # echo "/usr/local/lib/" >> /etc/ld.so.conf
    # exit
    $ sudo ldconfig
    $ cat /etc/ld.so.conf
    
    $ ./configure --prefix=/usr/local/ # OR
    $ ./configure --prefix=/usr/local/ \
    --enable-rohc-debug \
    --enable-fail-on-warning \
    --enable-fortify-sources \
    --enable-app-performance \
    --enable-app-sniffer \
    --enable-app-stats \
    --enable-doc \
    --enable-rohc-tests \
    --enable-rohc-tests-valgrind \
    --enable-examples
    $ make all
    $ make check
    $ sudo make install

test ROHC

    $ sudo yum install lynx
    [Lynx 2.8.8](http://lynx.invisible-island.net/release/)
    $ ./simple_generate.sh --prefix=/usr/local
    $ ./configure --prefix=/usr/local
    $ make
    $ sudo make install
    $ sudo find / -iname cmocka.pc
    $ sudo cp /usr/local/lib/pkgconfig/rohc.pc /usr/lib64/pkgconfig/
    $ sudo ldconfig
    
    [pcap example](https://github.com/markofu/hackeire)

    $ gcc -o example_rohc_comp_decomp -Wall $(pkg-config rohc --cflags) example_rohc_comp_decomp.c $(pkg-config rohc --libs)
    $ gcc -o example_rohc_comp_decomp -Wall -I/usr/local/include example_rohc_comp_decomp.c -L/usr/local/lib -lrohc
    $ gcc -o example_rohc_comp_decomp -Wall -I./../src/common/  -I./../src/comp/ -I./../src/decomp/ example_rohc_comp_decomp.c -L./../src/.libs/ -lrohc
    
    $ gcc -o generate_ipv4_stream -Wall -I/usr/local/include -I./../ -I./../src/common/ generate_ipv4_stream.c -L/usr/local/lib -lrohc

## Reference

ROHC

- [RObust Header Compression (ROHC) library](https://www.openhub.net/p/rohc-library)
- [ROHC Library](https://rohc-lib.org/)
- [System requirements for building the ROHC library from sources](https://rohc-lib.org/wiki/doku.php?id=library-requirements)
- [Build and install the library from sources](https://rohc-lib.org/support/wiki/doku.php?id=library-install-sources)
- [didier-barvaux/rohc](https://github.com/didier-barvaux/rohc)
- [ROHC协议简介](http://blog.csdn.net/u013793399/article/details/51425161)
- [Linux下安装ROHC协议库并测试](http://blog.csdn.net/u013793399/article/details/51426548)
- [ROHC协议库测试与改造（一）](http://blog.csdn.net/u013793399/article/details/51472012)
- [ROHC协议库测试与改造（二）](http://blog.csdn.net/u013793399/article/details/51537089)
- [报头压缩-Linux下安装ROHC协议库](http://blog.csdn.net/baidu_35692628/article/details/71305814)

tcpdump & libpcap

- [tcpdump & libpcap](http://www.tcpdump.org/)
- [the-tcpdump-group/libpcap](https://github.com/the-tcpdump-group/libpcap)

cmocka

- [cmocka - unit testing framework for C](https://cmocka.org/)
- [projects/cmocka.git](https://git.cryptomilk.org/projects/cmocka.git/)
- [clibs/cmocka](https://github.com/clibs/cmocka)
- [elventear/cmocka](https://github.com/elventear/cmocka)
- [cmocka编译安装](http://blog.csdn.net/kaka09life/article/details/69666101)

rohc-lib

    git clone https://github.com/didier-barvaux/rohc.git
    git remote add codeclub git@code-sh.huawei.com:z00416007/rohc-lib.git
    git push -u codeclub --all
    git push -u codeclub --tags
    git clone git@code-sh.huawei.com:z00416007/rohc-lib.git

- [The magic behind configure, make, make install](https://robots.thoughtbot.com/the-magic-behind-configure-make-make-install)

-I/usr/local/include

    [bean@SHA1000154646 rohc-lib]$ ll /usr/local/include/rohc/
    total 84K
    drwxr-xr-x. 2 root 4.0K Aug  8 14:36 ./
    drwxr-xr-x. 3 root 4.0K Aug  8 14:36 ../
    -rw-r--r--. 1 root  13K Aug  8 14:36 rohc_buf.h
    -rw-r--r--. 1 root  15K Aug  8 14:36 rohc_comp.h
    -rw-r--r--. 1 root  16K Aug  8 14:36 rohc_decomp.h
    -rw-r--r--. 1 root 7.1K Aug  8 14:36 rohc.h
    -rw-r--r--. 1 root 4.9K Aug  8 14:36 rohc_packets.h
    -rw-r--r--. 1 root 1.5K Aug  8 14:36 rohc_time.h
    -rw-r--r--. 1 root 4.4K Aug  8 14:36 rohc_traces.h

-L/usr/local/lib -lrohc

    [bean@SHA1000154646 rohc-lib]$ ll /usr/local/lib | grep rohc
    -rwxr-xr-x.  1 root  908 Aug  8 14:36 librohc.la*
    lrwxrwxrwx.  1 root   16 Aug  8 14:36 librohc.so -> librohc.so.3.0.0*
    lrwxrwxrwx.  1 root   16 Aug  8 14:36 librohc.so.3 -> librohc.so.3.0.0*
    -rwxr-xr-x.  1 root 2.1M Aug  8 14:36 librohc.so.3.0.0*
