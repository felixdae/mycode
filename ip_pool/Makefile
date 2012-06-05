#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements. See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership. The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the License for the
# specific language governing permissions and limitations
# under the License.
#

CXXFLAGS := -g -m32 -Wall

TARGETDIR=GNU-x86-Window

INCLUDE = $(ACCOUNT_INCLUDE) $(DELIVERY_INCLUDE)

LIB = $(THRIFT_LIB) $(FRAMEWORK) $(LIBRARY2) $(MCRYPT) $(LIBXML) $(APR_LIB) $(MYSQL)

ippool: $(TARGETDIR) ippool_client
		
ippool_client: IPPool.cpp CIPPool.h CIPPool.cpp
	$(CXX) $(CXXFLAGS) -o BcIpPool $(INCLUDE) IPPool.cpp CIPPool.cpp $(LIB) -lstdc++ -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -lcrypt -L/usr/local/zlib/lib/ -lz -static

# static link bug
#0  0x00000000 in ?? ()
#1  0x0821e7b9 in __cxa_guard_release ()
#2  0x081d10c1 in get_locale_mutex () at ../../../../../libstdc++-v3/src/locale_init.cc:42
#3  0x081d218b in locale (this=0x83cdd9c) at ../../../../../libstdc++-v3/src/locale_init.cc:215
#4  0x081cd927 in Init (this=0x83cd87c) at /usr/src/packages/BUILD/gcc-4.3.3-20081022/obj-x86_64-suse-linux/x86_64-suse-linux/32/libstdc++-v3/include/streambuf:447
#5  0x081c7dad in __static_initialization_and_destruction_0 ()
#6  0x081c7de9 in global constructors keyed to _ZN11c2cplatform7library4util13CStringHelper7RandStrEPcj ()
#7  0x082c67dd in __do_global_ctors_aux ()
#8  0x08048215 in _init ()
#9  0x0824dca2 in __libc_csu_init (argc=9, argv=0xffe4f444, envp=0xffe4f46c) at elf-init.c:79
#10 0x0824d764 in __libc_start_main (main=0x80488ee <main>, argc=9, ubp_av=0xffe4f444, init=0x824dc50 <__libc_csu_init>, fini=0x824dc10 <__libc_csu_fini>, rtld_fini=0, stack_end=0xffe4f43c) at libc-start.c:179
#11 0x08048261 in _start () at ../sysdeps/i386/elf/start.S:119
#the solution
#http://gcc.gnu.org/bugzilla/show_bug.cgi?id=33960


clean:
	$(RM) -r BcIpPool

$(TARGETDIR):
	mkdir -p $(TARGETDIR)
