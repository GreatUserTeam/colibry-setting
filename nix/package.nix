{ lib
, stdenv
, cmake
, ninja
, qtbase
, wrapQtAppsHook
}:

stdenv.mkDerivation {
  pname = "colibry-settings";
  version = "0.2.0";

  src = lib.cleanSource ./..;

  nativeBuildInputs = [
    cmake
    ninja
    wrapQtAppsHook
  ];

  buildInputs = [
    qtbase
  ];

  cmakeFlags = [
    "-DColibry_BUILD_TESTS=OFF"
  ];

  meta = with lib; {
    description = "Settings for the Colibry";
    license = licenses.mit;
    platforms = platforms.linux;
  };
}
