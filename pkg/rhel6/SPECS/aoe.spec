Name:		aoe
Vendor:		Coraid, Inc.
Packager:	Ed Cashin <ecashin@coraid.com>
Version:	83
Release:	1%{?dist}
License:	GPLv2
URL:		http://support.coraid.com/support/linux/
Source0:	%{name}6-%{version}.tar.gz
Source1:	%{name}.files
Source2:	%{name}.conf
Source3:	%{name}-%{version}-nodepmod.diff
BuildRequires:	%kernel_module_package_buildreqs
Group:		System Environment/Kernel
Summary:	%{name} module for Linux
Requires:	aoetools
patch0:		%{SOURCE3}

%description
Coraid ATA over Ethernet (AoE) Linux initiator driver.

# add percent-sign-plus-trace to understand what's going on

%kernel_module_package -f %{SOURCE1} default

%prep
%{?kmodtool_check}
if test dev_interested = 'SURE!'; then
	cat <<EOF
kmodtool: %{kmodtool}
kmod_version: %{kmod_version}
kmod_name: %{kmod_name}
latest_kernel: %{latest_kernel}
kernel_source: %{kernel_source default}
kverrel: %{kverrel}
buildroot: %{buildroot}
EOF
fi
%setup -q -n aoe6-%{version}
%patch0

%build
%{__make} linux/drivers/block/%{name}/%{name}.ko \
  KDIR=%{kernel_source default} \
  INSTDIR=%{buildroot}/lib/modules/%{kverrel}/extra/%{name} \
  %{?_smp_mflags}

%install
%{__make} install_modobj \
  KDIR=%{kernel_source default} \
  INSTDIR=%{buildroot}/lib/modules/%{kverrel}/extra/%{name} \
  %{?_smp_mflags}
# find %{buildroot} -type f -name '*.ko' -print0 | xargs -0 chmod u+x
install -m 644 -D %{SOURCE2} $RPM_BUILD_ROOT/etc/depmod.d/%{name}.conf
