Name:           HikNetExtractor
Summary:        HikNetExtractor for Hikvision
Version:        1.0.7

%if 0%{?mageia}
Release:        %mkrel 1
%else
Release:        1
%endif

License:        GPLv3
Group:          Video/Players
URL:            https://github.com/surfzoid/HNEWizard
Source0:        https://github.com/surfzoid/HNEWizard/archive/%{version}/HNEWizard-%{version}.tar.gz
# List of additional build dependencies
%if 0%{?mageia}
BuildRequires:  qtbase5-common-devel
BuildRequires:  pkgconfig(Qt5Widgets)
%else
%if 0%{?suse_version}
BuildRequires:  libqt5-qtbase-devel
%else
BuildRequires:  pkgconfig(Qt5)
%endif
%endif
BuildRequires:  wget
BuildRequires:  unzip

Requires: qtbase5-common
 
%description
A wizard to configure and install HikNetExtractor https://github.com/surfzoid/HikNetExtractor
 
%prep
%autosetup -n HNEWizard-%{version}
 
%build
%if 0%{?suse_version}
#find /|grep -i qmake
/usr/bin/qmake-qt5
%else
%qmake_qt5
%endif
%make_build
 
%install
%make_install INSTALL_ROOT=%{buildroot}%{_datadir}
 
%find_lang %{name} --with-qt --all-name

mkdir -p %{buildroot}%{_datadir}/HikNetExtractor/template/

wget https://github.com/surfzoid/HikNetExtractor/archive/refs/heads/main.zip
unzip -o ./main.zip
cd ./HikNetExtractor-main/
cp -fv ./*.py %{buildroot}%{_datadir}/HikNetExtractor/template/
cp -fv ./LICENSE %{buildroot}%{_datadir}/HikNetExtractor/template/
cp -fv ./README.md %{buildroot}%{_datadir}/HikNetExtractor/template/

%post

%clean
rm -rf %buildroot
chmod -R ug+rw %{_rpmdir}
chmod -R ug+rw %{_srcrpmdir}
 
%files
%license LICENSE
%if 0%{?suse_version}
%{_datadir}/doc/README.md
%else
%doc README.md
%endif
%{_bindir}/HikNetExtractor
%{_datadir}/applications/%{name}.desktop
%{_datadir}/HikNetExtractor/
%{_datadir}/icons/hiknetextractor.png

%changelog
* Mon Feb 06 2023 surfzoid@gmail.com
+ First release.