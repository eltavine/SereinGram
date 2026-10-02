{
  description = "SereinGram, an open-source Telegram client based on Telegram Desktop";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    # Telegram Desktop builds from its submodules.
    self.submodules = true;
  };

  outputs =
    { self, nixpkgs }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});
      mkSereingram = pkgs: pkgs.callPackage ./packaging/nix/package.nix { src = self; };
    in
    {
      packages = forAllSystems (pkgs: rec {
        sereingram = mkSereingram pkgs;
        default = sereingram;
      });

      overlays.default = final: _prev: { sereingram = mkSereingram final; };

      checks = forAllSystems (pkgs: {
        sereingram = (mkSereingram pkgs).override { testCredentials = true; };
      });

      formatter = forAllSystems (pkgs: pkgs.nixfmt);
    };
}
