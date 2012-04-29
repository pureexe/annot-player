# rpm.spec
# 4/4/2012
# See: http://fedoraproject.org/wiki/How_to_create_an_RPM_package
# See: http://dl.atrpms.net/all/vlc.spec
Name:	annot-browser
Version:	0.1.2.1
Release:	1%{?dist}
Summary:	Web browser with proxy.

Group:		Applications/Network
License:	GPLv3
URL:		http://annot.me/
Source0:	%{name}-%{version}.tar.bz2

BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires: qt, qt-x11, qtwebkit
BuildRequires: qt-devel, qtwebkit-devel
BuildRequires: vlc-devel
BuildRequires: lua-devel
BuildRequires: zlib-devel

Requires:	lua, qt, qt-x11, qtwebkit, vlc, zlib

%description
 Annot Browser is a web broswer with built-in proxy for visiting website such as nicovideo.jp.
 Official wiki: http://annot.me/w.

%prep
%setup -q

%build
qmake-qt4 -config release PREFIX=/usr project/browser/browser.pro
make %{?_smp_mflags}

%install
rm -Rf $RPM_BUILD_ROOT
make install INSTALL_ROOT=$RPM_BUILD_ROOT

%clean
rm -Rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc COPYING ChangeLog
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/kde4/apps/solid/actions/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png

%changelog

# EOF
