%global __provides_exclude_from ^%{_datadir}/%{name}/lib/.*$
%global __requires_exclude ^(libonnxruntime.*|libonnx.*|libprotobuf.*|libabsl.*|libkissfft.*|libXNNPACK.*|libcpuinfo.*|libnsync.*|libpthreadpool.*|libre2.*|libz.*|libdate.*)$
Name:       ru.repeater.repeater
Summary:    My Aurora OS Application
Version:    0.1
Release:    1
License:    BSD-3-Clause
URL:        https://auroraos.ru
Source0:    %{name}-%{version}.tar.bz2

Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(auroraapp)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(Qt5Multimedia)
BuildRequires: conan

%description
Offline dictaphone that transcribes speech to text on device.

%prep
%autosetup

%build
CONAN_LIB_DIR="%{_builddir}/conan-libs/"
%{set_build_flags}
rm -f "$CONAN_LIB_DIR/conanrun.sh"

# Обходим враппер make из Aurora SDK, заставляя CMake использовать системный make
export CMAKE_MAKE_PROGRAM=/usr/bin/make
export MAKE=/usr/bin/make

# Запускаем Conan с системным PATH
PATH=/usr/bin:$PATH conan-install-if-modified --source-folder="%{_sourcedir}/.." \
    --output-folder="$CONAN_LIB_DIR" -vwarning --build=missing

export PKG_CONFIG_PATH="$CONAN_LIB_DIR":$PKG_CONFIG_PATH


%qmake5
%make_build

%install
%make_install
EXECUTABLE="%{buildroot}/%{_bindir}/%{name}"
CONAN_LIB_DIR="%{_builddir}/conan-libs/"
SHARED_LIBRARIES="%{buildroot}/%{_datadir}/%{name}/lib"
mkdir -p "$SHARED_LIBRARIES"
conan-deploy-libraries "$EXECUTABLE" "$CONAN_LIB_DIR" "$SHARED_LIBRARIES"

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
