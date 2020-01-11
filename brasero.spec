Name:      brasero
Version:   3.12.1
Release:   2%{?dist}
Summary:   Gnome CD/DVD burning application
Group:     Applications/Multimedia
# see https://bugzilla.gnome.org/show_bug.cgi?id=683503
License:   GPLv3+
URL:       http://www.gnome.org/projects/brasero/
#VCS: git:git://git.gnome.org/brasero
Source0:   https://download.gnome.org/sources/brasero/3.12/%{name}-%{version}.tar.xz
# https://bugzilla.gnome.org/show_bug.cgi?id=647838
# update man page
Patch0: 0001-Update-the-man-page.patch

BuildRequires:  gtk3-devel >= 2.99.0
BuildRequires:  glib2-devel >= 2.15.6
BuildRequires:  gettext intltool gtk-doc
BuildRequires:  desktop-file-utils
BuildRequires:  gstreamer1-devel >= 0.11.92
BuildRequires:  gstreamer1-plugins-base-devel >= 0.11.92
BuildRequires:  totem-pl-parser-devel >= 2.22.0
BuildRequires:  libnotify-devel >= 0.7.0
BuildRequires:  libxml2-devel >= 2.6.0
BuildRequires:  dbus-glib-devel >= 0.7.2
BuildRequires:  libxslt
BuildRequires:  /usr/bin/appstream-util
BuildRequires:  libburn-devel >= 0.4.0
BuildRequires:  libisofs-devel >= 0.6.4
BuildRequires:  nautilus-devel >= 2.22.2
BuildRequires:  libSM-devel
BuildRequires:  libcanberra-devel
BuildRequires:  gobject-introspection-devel
BuildRequires:  tracker-devel
BuildRequires:  gnome-common
BuildRequires:  itstool
BuildRequires:  yelp-tools

Requires:  dvd+rw-tools
Requires:  cdrecord
Requires:  mkisofs
Requires:  cdda2wav
Requires:  %{name}-libs%{?_isa} = %{version}-%{release}
%ifnarch s390 s390x
Requires:  cdrdao
%endif

%description
Simple and easy to use CD/DVD burning application for the Gnome
desktop.


%package   libs
Summary:   Libraries for %{name}
Group:     System Environment/Libraries
Obsoletes: nautilus-cd-burner-libs < 2.25.4


%description libs
The %{name}-libs package contains the runtime shared libraries for
%{name}.


%package   nautilus
Summary:   Nautilus extension for %{name}
Group:     User Interface/Desktops

Provides:  nautilus-cd-burner = %{version}-%{release}
Obsoletes: nautilus-cd-burner < 2.25.4
Requires:  %{name} = %{version}-%{release}

%description nautilus
The %{name}-nautilus package contains the brasero nautilus extension.


%package        devel
Summary:        Headers for developing programs that will use %{name}
Group:          Development/Libraries
Requires:       %{name}-libs%{?_isa} = %{version}-%{release}
Obsoletes:      nautilus-cd-burner-devel < 2.25.4


%description devel
This package contains the static libraries and header files needed for
developing brasero applications.


%prep
%setup -q
%patch0 -p1


%build
%configure \
        --enable-nautilus \
        --enable-libburnia \
        --enable-search \
        --enable-playlist \
        --enable-preview \
        --enable-inotify \
        --disable-caches \
        --disable-static
sed -i -e 's! -shared ! -Wl,--as-needed\0!g' libtool
make %{?_smp_mflags}


%install
make install DESTDIR=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -type f -name "*.la" -exec rm -f {} ';'
%find_lang %{name}

sed -i 's/cd:x/cd;x/' $RPM_BUILD_ROOT%{_datadir}/applications/%{name}.desktop

desktop-file-install --vendor ""                   \
    --dir $RPM_BUILD_ROOT%{_datadir}/applications  \
    $RPM_BUILD_ROOT%{_datadir}/applications/%{name}.desktop

desktop-file-install --vendor ""                   \
    --dir $RPM_BUILD_ROOT%{_datadir}/applications  \
    $RPM_BUILD_ROOT%{_datadir}/applications/%{name}-nautilus.desktop


%check
appstream-util validate-relax --nonet %{buildroot}%{_datadir}/appdata/%{name}.appdata.xml


%post
touch --no-create %{_datadir}/icons/hicolor || :
touch --no-create %{_datadir}/mime/packages &> /dev/null || :
update-desktop-database &> /dev/null ||:


%post libs -p /sbin/ldconfig


%postun
update-desktop-database &> /dev/null ||:
if [ $1 -eq 0 ]; then
  touch --no-create %{_datadir}/icons/hicolor || :
  gtk-update-icon-cache %{_datadir}/icons/hicolor &> /dev/null || :
  glib-compile-schemas %{_datadir}/glib-2.0/schemas &> /dev/null || :
  touch --no-create %{_datadir}/mime/packages &> /dev/null || :
  update-mime-database %{?fedora:-n} %{_datadir}/mime &> /dev/null || :
fi


%postun libs -p /sbin/ldconfig


%posttrans
gtk-update-icon-cache %{_datadir}/icons/hicolor &> /dev/null || :
glib-compile-schemas %{_datadir}/glib-2.0/schemas &> /dev/null || :
update-mime-database %{?fedora:-n} %{_datadir}/mime &> /dev/null || :


%files -f %{name}.lang
%doc AUTHORS COPYING NEWS README
%{_mandir}/man1/%{name}.*
%{_bindir}/*
%{_libdir}/brasero3
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/appdata/%{name}.appdata.xml
%{_datadir}/help/*
%{_datadir}/icons/hicolor/*/apps/*
%{_datadir}/mime/packages/*
%{_datadir}/GConf/gsettings/brasero.convert
%{_datadir}/glib-2.0/schemas/org.gnome.brasero.gschema.xml

%files libs
%{_libdir}/*.so.*
%{_libdir}/girepository-1.0/*.typelib

%files nautilus
%{_libdir}/nautilus/extensions-3.0/*.so
%{_datadir}/applications/brasero-nautilus.desktop


%files devel
%doc %{_datadir}/gtk-doc/html/libbrasero-media
%doc %{_datadir}/gtk-doc/html/libbrasero-burn
%doc ChangeLog
%{_libdir}/*.so
%{_libdir}/pkgconfig/*.pc
%{_includedir}/brasero3
%{_datadir}/gir-1.0/*.gir


%changelog
* Tue May 19 2015 David King <dking@redhat.com> - 3.12.1-2
- Rebuild for totem-pl-parser (#1222884)

* Mon May  4 2015 Richard Hughes <rhughes@redhat.com> - 3.12.1-1
- Update to 3.12.1
- Resolves: #1174577

* Sat Aug  2 2014 Peter Robinson <pbrobinson@redhat.com> 3.8.0-8
- Fix dependencies on own library

* Fri Jan 24 2014 Daniel Mach <dmach@redhat.com> - 3.8.0-7
- Mass rebuild 2014-01-24

* Tue Jan  7 2014 Zeeshan Ali <zeenix@redhat.com> - 3.8.0-6
- Add dep on own library (related: #1045140)

* Fri Dec 27 2013 Daniel Mach <dmach@redhat.com> - 3.8.0-5
- Mass rebuild 2013-12-27

* Thu Dec  5 2013 Zeeshan Ali <zeenix@redhat.com> - 3.8.0-4
- Complete translations (related: #1030319)

* Thu Nov  7 2013 Zeeshan Ali <zeenix@redhat.com> - 3.8.0-3
- Fix ISRC handling (related: #861191)

* Tue Oct  8 2013 Matthias Clasen <mclasen@redhat.com> - 3.8.0-2
- Updated man page (related: #948920)

* Tue Mar 26 2013 Kalev Lember <kalevlember@gmail.com> - 3.8.0-1
- Update to 3.8.0

* Wed Mar  6 2013 Matthias Clasen <mclasen@redhat.com> - 3.7.91-1
- Update to 3.7.91

* Sun Jan 27 2013 Kalev Lember <kalevlember@gmail.com> - 3.6.1-2
- Build with tracker 0.16 API

* Mon Nov 12 2012 Tomas Bzatek <tbzatek@redhat.com> - 3.6.1-1
- Update to 3.6.1

* Wed Oct 10 2012 Tomas Bzatek <tbzatek@redhat.com> - 3.6.0-2
- Fix BD media disc copy

* Tue Sep 25 2012 Richard Hughes <hughsient@gmail.com> - 3.6.0-1
- Update to 3.6.0

* Wed Sep 19 2012 Richard Hughes <hughsient@gmail.com> - 3.5.92-1
- Update to 3.5.92

* Wed Jul 18 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.4.1-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_18_Mass_Rebuild

* Tue Apr 17 2012 Kalev Lember <kalevlember@gmail.com> - 3.4.1-1
- Update to 3.4.1

* Tue Mar 27 2012 Richard Hughes <hughsient@gmail.com> - 3.4.0-1
- Update to 3.4.0

* Wed Mar 21 2012 Kalev Lember <kalevlember@gmail.com> - 3.3.92-1
- Update to 3.3.92

* Tue Mar  6 2012 Matthias Clasen <mclasen@redhat.com> - 3.3.91-2
- Rebuild against new tracker

* Tue Mar  6 2012 Matthias Clasen <mclasen@redhat.com> - 3.3.91-1
- Update to 3.3.91

* Tue Feb 28 2012 Matthias Clasen <mclasen@redhat.com> - 3.2.0-3
- Rebuild against new tracker

* Thu Jan 12 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.2.0-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_17_Mass_Rebuild

* Tue Sep 27 2011 Ray <rstrode@redhat.com> - 3.2.0-1
- Update to 3.2.0

* Fri May  6 2011 Christopher Aillon <caillon@redhat.com> - 3.0.0-1
- Update scriptlets per packaging guidelines

* Mon Apr  4 2011 Matthias Clasen <mclasen@redhat.com> - 3.0.0-1
- Update to 3.0.0

* Fri Mar 25 2011 Matthias Clasen <mclasen@redhat.com> - 2.91.93-1
- Update to 2.91.93

* Tue Mar  8 2011 Matthias Clasen <mclasen@redhat.com> - 2.91.91-1
- Update to 2.91.91

* Tue Feb 22 2011 Matthias Clasen <mclasen@redhat.com> - 2.91.90-1
- Update to 2.91.90

* Thu Feb 10 2011 Matthias Clasen <mclasen@redhat.com> - 2.91.6-4
- Rebuild against newer gtk

* Mon Feb 07 2011 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 2.91.6-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_15_Mass_Rebuild

* Sun Feb 06 2011 Bastien Nocera <bnocera@redhat.com> 2.91.6-2
- Fix GTK+ name in pkg-config file

* Wed Feb  2 2011 Christopher Aillon <caillon@redhat.com> - 2.91.6-1
- Update to 2.91.6

* Thu Jan 13 2011 Matthias Clasen <mclasen@redhat.com> - 2.91.5-2
- Move girs to -devel

* Mon Jan 10 2011 Matthias Clasen <mclasen@redhat.com> - 2.91.5-1
- Update to 2.91.5

* Fri Jan  7 2011 Matthias Clasen <mclasen@redhat.com> - 2.91.4.2-1
- Update to 2.91.4.2

* Fri Dec  3 2010 Matthias Clasen <mclasen@redhat.com> - 2.91.3-1
- Update to 2.91.3

* Thu Nov 11 2010 Matthias Clasen <mclasen@redhat.com> - 2.91.2-1
- Update to 2.91.2

* Wed Nov  3 2010 Matthias Clasen <mclasen@redhat.com> - 2.91.2-0.2.gitcede364
- Rebuild against libnotify 0.7.0

* Mon Nov  1 2010 Matthias Clasen <mclasen@redhat.com> - 2.91.2-0.1.gitcede364
- Git snapshot that builds against gtk3

* Wed Oct  6 2010 Matthias Clasen <mclasen@redhat.com> - 2.91.0-1
- Update to 2.91.0

* Tue Sep 28 2010 Matthias Clasen <mclasen@redhat.com> - 2.32.0-1
- Update to 2.32.0

* Wed Sep 22 2010 Matthias Clasen <mclasen@redhat.com> - 2.31.92-1
- Update to 2.31.92

* Tue Aug 31 2010 Matthias Clasen <mclasen@redhat.com> - 2.31.91-1
- Update to 2.31.91

* Thu Aug 19 2010 Matthias Clasen <mclasen@redhat.com> - 2.31.90-1
- Update to 2.31.90

* Thu Jul 15 2010 Colin Walters <walters@verbum.org> - 2.31.5-2
- Rebuild with new gobject-introspection

* Tue Jul 13 2010 Matthias Clasen <mclasen@redhat.com> - 2.31.5-1
- Update to 2.31.5

* Wed Jun 30 2010 Matthias Clasen <mclasen@redhat.com> - 2.31.4-1
- Update to 2.31.4

* Tue Jun  8 2010 Matthias Clasen <mclasen@redhat.com> - 2.31.3-1
- Update to 2.31.3

* Thu May 27 2010 Matthias Clasen <mclasen@redhat.com> - 2.31.2-1
- Update to 2.31.2

* Tue Apr 27 2010 Matthias Clasen <mclasen@redhat.com> - 2.30.1-1
- Update to 2.30.1
- Spec file cleanups

* Mon Mar 29 2010 Matthias Clasen <mclasen@redhat.com> - 2.30.0-1
- Update to 2.30.0

* Thu Mar 11 2010 Matthias Clasen <mclasen@redhat.com> - 2.29.92-1
- Update to 2.29.92

* Wed Mar  3 2010 Matthias Clasen <mclasen@redhat.com> - 2.29.91-2
- Fix a nautilus cd-burner crash

* Mon Feb 22 2010 Matthias Clasen <mclasen@redhat.com> - 2.29.91-1
- Update to 2.29.91

* Wed Feb 10 2010 Bastien Nocera <bnocera@redhat.com> 2.29.90-1
- Update to 2.29.90

* Tue Jan 26 2010 Bastien Nocera <bnocera@redhat.com> 2.29.4-2
- Add patch for new totem-pl-parser API
- Fix introspection building

* Tue Dec 22 2009 Matthias Clasen <mclasen@redhat.com> 2.29.4-1
- Update to 2.29.4

* Wed Dec  2 2009 Bastien Nocera <bnocera@redhat.com> 2.29.3-2
- Make libbeagle dep more automatic

* Tue Dec 01 2009 Bastien Nocera <bnocera@redhat.com> 2.29.3-1
- Update to 2.29.3

* Thu Nov 12 2009 Matthias Clasen <mclasen@redhat.com> 2.28.2-3
- Obsolete nautilus-cd-burner-devel and -libs as well

* Mon Oct 26 2009 Matthias Clasen <mclasen@redhat.com> 2.28.2-2
- Avoid a stray underline in a button label

* Tue Oct 20 2009 Matthias Clasen <mclasen@redhat.com> 2.28.2-1
- Update to 2.28.2

* Wed Oct 07 2009 Bastien Nocera <bnocera@redhat.com> 2.28.1-2
- Fix command-line parsing (#527484)

* Mon Oct  5 2009 Matthias Clasen <mclasen@redhat.com> - 2.28.1-1
- Update to 2.28.1, fixes a number of crashes and other serious bugs:
 - Fix a crash when we try to download a missing gstreamer plugin through PK
 - Don't fail if a drive cannot be checksumed after a burn
 - Fix a data corruption when libisofs was used for a dummy session
 - Fix #596625: brasero crashed with SIGSEGV in brasero_track_data_cfg_add
 - Fix progress reporting
 ...

* Fri Oct  2 2009 Matthias Clasen <mclasen@redhat.com> - 2.28.0-2
- Fix ejecting after burning

* Tue Sep 22 2009 Matthias Clasen <mclasen@redhat.com> - 2.28.0-1
- Update to 2.28.0

* Fri Sep 11 2009 Karsten Hopp <karsten@redhat.com> 2.27.92-2
- fix requirements on s390, s390x where we don't have cdrdao

* Mon Sep  7 2009 Matthias Clasen <mclasen@redhat.com> - 2.27.92-1
- Update to 2.27.92

* Tue Aug 25 2009 Matthias Clasen <mclasen@redhat.com> - 2.27.91-1
- Update to 2.27.91

* Mon Aug 10 2009 Matthias Clasen <mclasen@redhat.com> - 2.27.90-1
- Update to 2.27.90

* Mon Aug  3 2009 Matthias Clasen <mclasen@redhat.com> - 2.27.5-2
- Fix a nautilus segfault when burning  

* Tue Jul 28 2009 Matthias Clasen <mclasen@redhat.com> - 2.27.5-1
- Update to 2.27.5

* Sun Jul 26 2009 Matthias Clasen <mclasen@redhat.com> - 2.27.4-3
- Move ChangeLog to -devel to save some space

* Fri Jul 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 2.27.4-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_12_Mass_Rebuild

* Tue Jul 14 2009 Matthias Clasen <mclasen@redhat.com> 2.27.4-1
- Update to 2.27.4

* Tue Jun 16 2009 Matthias Clasen <mclasen@redhat.com> 2.27.3-1
- Update to 2.27.3

* Wed May 27 2009 Bastien Nocera <bnocera@redhat.com> 2.27.2-1
- Update to 2.27.2

* Tue May 26 2009 Bastien Nocera <bnocera@redhat.com> 2.27.1-2
- Add missing unique-devel BR

* Mon May 18 2009 Bastien Nocera <bnocera@redhat.com> 2.27.1-1
- Update to 2.27.1

* Fri May  1 2009 Bill Nottingham <notting@redhat.com> - 2.26.1-3
- require main package in brasero-nautilus (#498632)

* Fri Apr 17 2009 Denis Leroy <denis@poolshark.org> - 2.26.1-2
- Obsoletes nautilus-cd-burner

* Tue Apr 14 2009 Matthias Clasen <mclasen@redhat.com> - 2.26.1-1
- Update to 2.26.1
- See http://download.gnome.org/sources/brasero/2.26/brasero-2.26.1.news

* Mon Apr 13 2009 Denis Leroy <denis@poolshark.org> - 2.26.0-2
- Removed duplicate desktop source

* Sun Mar 29 2009 Matthias Clasen <mclasen@redhat.com> - 2.26.0-1
- Update to 2.26.0

* Mon Mar 02 2009 - Bastien Nocera <bnocera@redhat.com> - 2.25.91.2-3
- Fix icon and Bugzilla component

* Mon Mar 02 2009 - Bastien Nocera <bnocera@redhat.com> - 2.25.91.2-2
- Fix regressions in burn:/// and blank media handling

* Tue Feb 24 2009 Denis Leroy <denis@poolshark.org> - 2.25.91.2-1
- Update to upstream 2.25.91.2
- Dvdcss patch upstreamed
- Split nautilus extension into subpackage (#485918)

* Mon Feb 23 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 2.25.90-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_11_Mass_Rebuild

* Sat Feb  7 2009 Denis Leroy <denis@poolshark.org> - 2.25.90-2
- Added patch to fix dynamic load of libdvdcss (#484413)

* Tue Feb  3 2009 Denis Leroy <denis@poolshark.org> - 2.25.90-1
- Update to upstream 2.25.90
- Split media library into separate RPM (#483754)
- Added patch to validate desktop files

* Tue Jan 20 2009 Denis Leroy <denis@poolshark.org> - 0.9.1-1
- Update to upstream 0.9.1
- Added development package

* Tue Dec 16 2008 Denis Leroy <denis@poolshark.org> - 0.8.3-1
- Update to upstream 0.8.4
- Enabled nautilus extension

* Mon Sep 15 2008 Denis Leroy <denis@poolshark.org> - 0.8.2-1
- Update to upstream 0.8.2

* Wed Aug 27 2008 Denis Leroy <denis@poolshark.org> - 0.8.1-1
- Update to upstream 0.8.1
- Desktop patch upstreamed

* Sun Jul  6 2008 Denis Leroy <denis@poolshark.org> - 0.7.91-1
- Update to unstable 0.7.91
- open flags patch upstreamed

* Wed Jun 11 2008 Denis Leroy <denis@poolshark.org> - 0.7.90-1
- Update to unstable 0.7.90
- Added patch to validate desktop file
- BRs updated

* Fri May 16 2008 Denis Leroy <denis@poolshark.org> - 0.7.1-4
- Rebuild for new totem-pl-parser

* Sat Feb 23 2008 Denis Leroy <denis@poolshark.org> - 0.7.1-3
- Fixed desktop mime field

* Mon Feb 18 2008 Fedora Release Engineering <rel-eng@fedoraproject.org> - 0.7.1-2
- Autorebuild for GCC 4.3

* Tue Jan 29 2008 Denis Leroy <denis@poolshark.org> - 0.7.1-1
- Update to 0.7.1 upstream, bugfix release

* Sun Dec 30 2007 Denis Leroy <denis@poolshark.org> - 0.7.0-1
- Update to upstream 0.7.0, updated BRs
- Forward-ported open() permission patch

* Mon Dec 10 2007 Denis Leroy <denis@poolshark.org> - 0.6.1-4
- Changed totem-devel req to totem-pl-parser-devel

* Sun Dec  9 2007 Denis Leroy <denis@poolshark.org> - 0.6.1-3
- Rebuild with new libbeagle

* Fri Nov  9 2007 Denis Leroy <denis@poolshark.org> - 0.6.1-2
- Rebuild to pick up new totem version (#361361)

* Sat Aug 25 2007 Denis Leroy <denis@poolshark.org> - 0.6.1-1
- Update to upstream version 0.6.1
- Filter UI patch is now upstream

* Fri Aug 17 2007 Denis Leroy <denis@poolshark.org> - 0.6.0-2
- Updated License tag
- Fixed open() O_CREAT problem

* Fri Aug 10 2007 Denis Leroy <denis@poolshark.org> - 0.6.0-1
- Update to 0.6.0
- Removed libburn support until it compiles against libisofs 0.2.8
- Fixed project URL
- Added patch to port to new Gtk+ tooltip interface
- Added patch to fix filter dialog crash

* Sun Jun  3 2007 Denis Leroy <denis@poolshark.org> - 0.5.2-4
- Removed beagle support for ppc64

* Tue May 22 2007 Denis Leroy <denis@poolshark.org> - 0.5.2-3
- Added umask 022 to scriptlets (#230781)

* Mon May 21 2007 Denis Leroy <denis@poolshark.org> - 0.5.2-2
- Rebuild to pick up new totem library

* Mon Feb 26 2007 Denis Leroy <denis@poolshark.org> - 0.5.2-1
- Update to 0.5.2
- Removed libisofs patch, now upstream

* Wed Jan 17 2007 Denis Leroy <denis@poolshark.org> - 0.5.1-2
- Added patch to support libisofs.so.4 and libburn.so.6

* Thu Nov 16 2006 Denis Leroy <denis@poolshark.org> - 0.5.1-1
- Update to 0.5.1

* Sun Oct 29 2006 Denis Leroy <denis@poolshark.org> - 0.5.0-1
- Update to 0.5.0
- Updated icon paths
- Added gconf schemas sections

* Tue Oct  3 2006 Denis Leroy <denis@poolshark.org> - 0.4.4-3
- fixed homepage URL

* Tue Sep 26 2006 Denis Leroy <denis@poolshark.org> - 0.4.4-2
- BRs cleanup

* Fri Sep 22 2006 Denis Leroy <denis@poolshark.org> - 0.4.4-1
- First version
foo
