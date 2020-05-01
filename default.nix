with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "raymarcher-dev-env";
  buildInputs = [
    pkgconfig gdb
  ];
}