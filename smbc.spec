Summary: Samba Commander
Name: smbc
Version: 1.2.2
Release: 0
Copyright: GPL
Group: Applications/File
Source:  %{name}-%{version}.tgz
BuildRoot: /var/tmp/%{name}
Packager: Rafal Michniewicz <rafim@data.pl>
URL: http://smbc.airm.net
Provides: smbc
Requires: samba 

%description
Samba Commander is a text mode SMBnet commander. In SMBC, you can browse the 
local network or you can use the search function to find the files. You can 
also download and upload files or all directories to your computer or create 
remote and local directories. SMBC has a resume function and supports 
UTF-8 characters.

%prep
%setup -q -n smbc-%{version}
LOCALEDIR=/usr/share/locale

%build
#./configure --prefix=/usr --with-debug
./configure --prefix=/usr
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/usr/share
mkdir -p $RPM_BUILD_ROOT/usr/share/man/man1
mkdir -p $RPM_BUILD_ROOT/usr/share/locale/pl/LC_MESSAGES/
mkdir -p $RPM_BUILD_ROOT/usr/share/locale/ru/LC_MESSAGES/

install -s -m 755 src/smbc $RPM_BUILD_ROOT/usr/bin/smbc
install -s -m 444 po/pl.gmo $RPM_BUILD_ROOT/usr/share/locale/pl/LC_MESSAGES/smbc.mo
install -s -m 444 po/ru.gmo $RPM_BUILD_ROOT/usr/share/locale/ru/LC_MESSAGES/smbc.mo
install -s -m 444 doc/smbc.1 $RPM_BUILD_ROOT/usr/share/man/man1/smbc.1
install -s -m 444 doc/smbcrc.1 $RPM_BUILD_ROOT/usr/share/man/man1/smbcrc.1

%clean
rm -rf $RPM_BUILD_ROOT

%files 
%defattr(-,root,root)
%doc INSTALL FAQ README ChangeLog doc/sample.smbcrc
%{_bindir}/smbc
%{_datadir}/locale/*/LC_MESSAGES/*.mo
%{_datadir}/man/man1/smbc.1.gz
%{_datadir}/man/man1/smbcrc.1.gz

%changelog

