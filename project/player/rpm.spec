# rpmspec
# 12/5/2011
# See: http://fedoraproject.org/wiki/How_to_create_an_RPM_package
# See: http://dl.atrpms.net/all/vlc.spec
Name:	annot-player
Version:	0.1.5.4
Release:	1%{?dist}
Summary:	Media and annotation player.

Group:		Applications/Multimedia
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
Annot Player is a software for sharing annotations while playing movies, music, pictures, and games (esp. Japanese AVG).
All annotations are saved onto the Cloud at http://annot.me.
Official wiki: http://annot.me/w.

%prep
%setup -q

%build
qmake-qt4 -config release PREFIX=/usr project/player/player.pro
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
