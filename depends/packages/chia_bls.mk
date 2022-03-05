package=chia_bls
$(package)_version=v20181101
# It's actually from https://github.com/Chia-Network/bls-signatures, but we have so many patches atm that it's forked
$(package)_download_path=https://github.com/codablock/bls-signatures/archive
$(package)_file_name=$($(package)_version).tar.gz
$(package)_sha256_hash=b3ec74a77a7b6795f84b05e051a0824ef8d9e05b04b2993f01040f35689aa87c
$(package)_dependencies=gmp cmake
#$(package)_patches=...TODO (when we switch back to https://github.com/Chia-Network/bls-signatures)

#define $(package)_preprocess_cmds
#  for i in $($(package)_patches); do patch -N -p1 < $($(package)_patch_dir)/$$$$i; done
#endef

define $(package)_set_vars
  $(package)_config_opts=-DCMAKE_INSTALL_PREFIX=$($(package)_staging_dir)/$(host_prefix)
  $(package)_config_opts+= -DCMAKE_PREFIX_PATH=$($(package)_staging_dir)/$(host_prefix)
  $(package)_config_opts+= -DSTLIB=ON -DSHLIB=OFF -DSTBIN=ON
  $(package)_config_opts+= -DBUILD_BLS_PYTHON_BINDINGS=0 -DBUILD_BLS_TESTS=0 -DBUILD_BLS_BENCHMARKS=0
  $(package)_config_opts_linux=-DOPSYS=LINUX -DCMAKE_SYSTEM_NAME=Linux
  $(package)_config_opts_darwin=-DOPSYS=MACOSX -DCMAKE_SYSTEM_NAME=Darwin
  $(package)_config_opts_mingw32=-DOPSYS=WINDOWS -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_SHARED_LIBRARY_LINK_C_FLAGS="" -DCMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS=""
  $(package)_config_opts_i686+= -DWSIZE=32
  $(package)_config_opts_x86_64+= -DWSIZE=64
  $(package)_config_opts_arm+= -DWSIZE=32
  $(package)_config_opts_armv7l+= -DWSIZE=32
  $(package)_config_opts_aarch64+= -DWSIZE=64
  $(package)_config_opts_riscv64+= -DWSIZE=64
  $(package)_config_opts_riscv32+= -DWSIZE=32
  $(package)_config_opts_s390x+= -DWSIZE=64
  $(package)_config_opts_mipsel+= -DWSIZE=32
  $(package)_config_opts_mips+= -DWSIZE=32
  $(package)_config_opts_powerppc64+= -DWSIZE=64
  $(package)_config_opts_powerppc64le+= -DWSIZE=64
  $(package)_config_opts_debug=-DDEBUG=ON -DCMAKE_BUILD_TYPE=Debug

  ifneq ($(darwin_native_toolchain),)
    $(package)_config_opts_darwin+= -DCMAKE_AR="$(host_prefix)/native/bin/$($(package)_ar)"
    $(package)_config_opts_darwin+= -DCMAKE_RANLIB="$(host_prefix)/native/bin/$($(package)_ranlib)"
  endif

  $(package)_cppflags+=-UBLSALLOC_SODIUM
endef

define $(package)_preprocess_cmds
  sed -i.old "s|GIT_REPOSITORY https://github.com/relic-toolkit/relic.git|URL \"../../relic-toolkit-$($(package)_relic_version).tar.gz\"|" src/CMakeLists.txt && \
  sed -i.old "s|GIT_TAG        .*RELIC_GIT_TAG.*|URL_HASH SHA256=$($(package)_relic_sha256_hash)|" src/CMakeLists.txt
endef

define $(package)_config_cmds
  export CC="$($(package)_cc)" && \
  export CXX="$($(package)_cxx)" && \
  export CFLAGS="$($(package)_cflags) $($(package)_cppflags)" && \
  export CXXFLAGS="$($(package)_cxxflags) $($(package)_cppflags)" && \
  export LDFLAGS="$($(package)_ldflags)" && \
  mkdir -p build && cd build && \
  $(host_prefix)/bin/cmake ../ $($(package)_config_opts)
endef

define $(package)_build_cmds
  cd build && \
  $(MAKE) $($(package)_build_opts)
endef

define $(package)_stage_cmds
  cd build && \
  $(MAKE) install
endef