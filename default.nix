with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "raymarcher-dev-env";
  buildInputs = [
    gtk3 pkgconfig gdb
  ];
}