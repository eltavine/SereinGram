{
  lib,
  stdenv,
  fetchurl,
  runCommand,
  cargo,
  rustc,
  git,
  pkg-config,
  cmake,
  openssl,
}:

let
  tlottie = fetchurl {
    url = "https://codeload.github.com/dkaraush/tlottie/tar.gz/92df98dc209bc39b1e567ec74a8c86a0af5239de";
    sha256 = "7855c45912e59c51fbc530af5e849d6867e52e4d64a0117815add728e5cbee92";
  };
  walletEngine = fetchurl {
    url = "https://codeload.github.com/i582/wallet-engine/tar.gz/e59e0d89d7ee90c388bf36e3334c5b276f396697";
    sha256 = "45d22ed481dc4fab07390abfa811c095c424e11c745e87b9d0d27b40efa17da2";
  };
  walletPatch = fetchurl {
    url = "https://raw.githubusercontent.com/desktop-app/patches/845a383f7abe8e44f92dfdcd566d603066ec2696/wallet-engine.patch";
    sha256 = "b40aba82c39a42d0526a85ecfa40d613baa57b1a2d6600e601a42a1576b3b822";
  };
  sources = builtins.fromJSON (builtins.readFile ../flatpak/tdesktop-rust-cargo-sources.json);
  vendor = runCommand "tdesktop-rust-vendor" { } (
    lib.concatMapStringsSep "\n" (
      source:
      if source.type == "archive" then
        let
          archive = fetchurl { inherit (source) url sha256; };
        in
        ''
          mkdir -p "$out/${source.dest}"
          tar xf ${archive} --strip-components=1 -C "$out/${source.dest}"
        ''
      else if source.type == "inline" then
        ''
          mkdir -p "$out/${source.dest}"
          cp ${builtins.toFile source.dest-filename source.contents} "$out/${source.dest}/${source.dest-filename}"
        ''
      else
        throw "Unsupported Cargo source type: ${source.type}"
    ) sources
  );
in
stdenv.mkDerivation {
  pname = "tdesktop-rust";
  version = "7.3.3";
  dontUnpack = true;
  dontConfigure = true;
  nativeBuildInputs = [
    cargo
    rustc
    git
    pkg-config
    cmake
  ];
  buildInputs = [ openssl ];

  buildPhase = ''
    runHook preBuild
    mkdir tlottie wallet-engine
    tar xf ${tlottie} --strip-components=1 -C tlottie
    tar xf ${walletEngine} --strip-components=1 -C wallet-engine
    git -C wallet-engine apply ${walletPatch}
    cp -r ${../rust} tdesktop_rust
    chmod -R u+w tdesktop_rust

    export CARGO_HOME="$TMPDIR/cargo-home"
    export CARGO_TARGET_DIR="$PWD/target"
    export CARGO_BUILD_JOBS="$NIX_BUILD_CORES"
    export CARGO_NET_OFFLINE=true
    mkdir -p "$CARGO_HOME"
    cat > "$CARGO_HOME/config.toml" <<EOF
    [source.crates-io]
    replace-with = "vendored-sources"
    [source.vendored-sources]
    directory = "${vendor}/cargo/vendor"
    EOF
    cargo build --offline --locked --release --manifest-path tdesktop_rust/Cargo.toml
    (
      cd tdesktop_rust
      cargo run --offline --locked --release \
        --manifest-path ../wallet-engine/bindgen/cpp/bindgen/Cargo.toml -- \
        --library --out-dir ../bindings ../target/release/libtdesktop_rust.so
    )
    runHook postBuild
  '';

  installPhase = ''
    runHook preInstall
    mkdir -p "$out/lib" "$out/include/tlottie" "$out/include/wallet_engine"
    cp target/release/libtdesktop_rust.a "$out/lib/"
    cp tlottie/include/tlottie.h "$out/include/tlottie/"
    cp bindings/* "$out/include/wallet_engine/"
    runHook postInstall
  '';

  meta = {
    description = "Combined Rust archive and wallet bindings for Telegram Desktop";
    license = with lib.licenses; [
      mit
      asl20
    ];
    platforms = lib.platforms.linux;
  };
}
