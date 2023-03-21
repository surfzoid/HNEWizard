Name:           HNEWizard
Summary:        HNEWizard for Hikvision
Version:        1.0.0

%if 0%{?mageia}
Release:        %mkrel 1
%else
Release:        1
%endif

License:        GPLv3
Group:          Video/Players
URL:            https://github.com/surfzoid/HNEWizard
Source0:        https://github.com/surfzoid/HNEWizard/archive/%{version}/%{name}-%{version}.tar.gz
# List of additional build dependencies
%if 0%{?mageia}
BuildRequires:  qtbase5-common-devel
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
A wizard to configure HikNetExtractor https://github.com/surfzoid/HikNetExtractor
 
%prep
%autosetup -n %{name}-%{version}
 
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
mkdir -p %{buildroot}%{_datadir}/HikNetExtractor/template/
wget https://github.com/surfzoid/HikNetExtractor/archive/refs/heads/main.zip
unzip -o ./HikNetExtractor-main.zip
cd ./HikNetExtractor-main/
cp -fv ./*.py %{buildroot}%{_datadir}/HikNetExtractor/template/
cp -fv ./LICENSE %{buildroot}%{_datadir}/HikNetExtractor/template/
cp -fv ./README.md %{buildroot}%{_datadir}/HikNetExtractor/template/
#Desktop file
cat <<EOT >%{buildroot}%{_datadir}/applications/HNEWizard.desktop
[Desktop Entry]
Version=1.0
Name=%{Name}
GenericName=%{Summary}
Comment=%{Summary}
Exec=HNEWizard %U
TryExec=%{_bindir}/HNEWizard
Icon=QtVsPlayer
Terminal=true
Type=Application
Categories=AudioVideo;Player;
Keywords=Player;Video;HikVision;Motion;
X-Desktop-File-Install-Version=0.26
Path=/usr/bin
Comment[fr_FR]=Assistant de configuration pour HikNetExtractor
Name[fr_FR]=%{Name}
GenericName[fr_FR]=%{Name}
EOT

%post


%clean
rm -rf %buildroot
chmod -R ug+rw %{_rpmdir}
chmod -R ug+rw %{_srcrpmdir}
 
%files
%license LICENSE
%doc README.md
%if 0%{?suse_version}
%{_datadir}/doc/HNEWizard/README.md
%endif
%{_bindir}/HNEWizard
%{_datadir}/applications/HNEWizard.desktop
%{_datadir}/icons/QtVsPlayer.png
%{_datadir}/HNEWizard/

%changelog
* Mon Feb 06 2023 surfzoid@gmail.com
+ First release.