%define sbind %{buildroot}%{_sbindir}
%define mand %{buildroot}%{_mandir}
%define udevrulesd %{buildroot}%{_sysconfdir}/udev/rules.d
%define docfiles COPYING HACKING NEWS README

Summary: ATA over Ethernet (AoE) utilities
Name: aoetools
Vendor: Coraid, Inc.
Packager: Ed Cashin <ecashin@coraid.com>
Version: 36
Release: 1%{dist}
URL: http://aoetools.sourceforge.net/
Source0: %{name}-%{version}.tar.gz
Source1: 60-aoe.rules
License: GPLv2
Group: Application/System

%description
The command-line utilities for using the aoe driver to work with
ATA over Ethernet storage targets on a Linux system.

%define debug_package %{nil}

%prep
%setup -n %{name}-%{version}

%build
make SBINDIR=%{sbind} MANDIR=%{mand}

%install
mkdir -p -m 755 %{sbind}/
mkdir -p -m 755 %{mand}/man8/
make install SBINDIR=%{sbind} MANDIR=%{mand}
mkdir -p -m 755 %{udevrulesd}/
install -m 644 %{SOURCE1} %{udevrulesd}

%files
%defattr(-,root,root)
%doc %docfiles
%attr(0755,root,root) %{_sbindir}/*
%{_mandir}/man8/*
%config %{_sysconfdir}/udev/rules.d/*

%changelog
* Thu Aug 15 2013 Ed Cashin <ecashin@coraid.com> - 36-1
- Update for v36

* Wed Jul 10 2013 Ed Cashin <ecashin@coraid.com> - 35-1
- Initial package for aoetools v35 plus udev rules
