{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  # Инструменты, необходимые для сборки
  nativeBuildInputs = [
    pkgs.gcc
    pkgs.pkg-config
    pkgs.gnumake
  ];

  # Библиотеки, которые мы берем из Nix (только SDL2)
  buildInputs = [
    pkgs.SDL2
    pkgs.SDL2.dev
    pkgs.linuxHeaders
    pkgs.binutils
    
  ];
  
  
  shellHook = ''
  export PKG_CONFIG_PATH="${pkgs.SDL2.dev}/lib/pkgconfig"
  # Создаем временную папку для pkg-config файлов
  mkdir -p $PWD/.tmp/pkgconfig
  ln -sf ${pkgs.SDL2.dev}/lib/pkgconfig/sdl2-compat.pc $PWD/.tmp/pkgconfig/SDL2.pc
  export PKG_CONFIG_PATH="$PWD/.tmp/pkgconfig:$PKG_CONFIG_PATH"
'';
}

