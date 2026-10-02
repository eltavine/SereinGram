# SereinGram never ships the official Telegram API credentials. Override
# apiId and apiHash with your own pair from https://my.telegram.org;
# testCredentials = true builds with the upstream test-only credentials, which
# is meant for automated checks, not for daily use.
{
  lib,
  stdenv,
  telegram-desktop,
  pango,
  tlottie,
  src,
  version ? null,
  apiId ? null,
  apiHash ? null,
  testCredentials ? false,
}:

let
  versionLines = lib.splitString "\n" (builtins.readFile (src + "/Telegram/build/version"));
  versionLine = lib.findFirst (lib.hasPrefix "AppVersionStr ") "AppVersionStr 0" versionLines;

  credentials =
    if apiId != null && apiHash != null then
      [
        (lib.cmakeFeature "TDESKTOP_API_ID" (toString apiId))
        (lib.cmakeFeature "TDESKTOP_API_HASH" apiHash)
      ]
    else
      lib.optionals testCredentials [ (lib.cmakeBool "TDESKTOP_API_TEST" true) ];

  unwrapped = telegram-desktop.unwrapped.overrideAttrs (
    finalAttrs: previousAttrs: {
      pname = "sereingram-unwrapped";
      version =
        if version != null then version else lib.trim (lib.removePrefix "AppVersionStr" versionLine);
      inherit src;

      # The upstream recipe adds these two for its own package name only.
      buildInputs =
        previousAttrs.buildInputs ++ lib.optionals stdenv.hostPlatform.isLinux [ pango ] ++ [ tlottie ];

      cmakeFlags =
        lib.filter (flag: !lib.hasPrefix "-DTDESKTOP_API_" flag) previousAttrs.cmakeFlags
        ++ credentials;

      preConfigure =
        (previousAttrs.preConfigure or "")
        + lib.optionalString (credentials == [ ]) ''
          echo "SereinGram never ships the official Telegram API credentials." >&2
          echo "Override apiId and apiHash with your own pair from https://my.telegram.org," >&2
          echo "or set testCredentials = true for automated checks." >&2
          exit 1
        '';

      passthru = builtins.removeAttrs previousAttrs.passthru [ "updateScript" ];

      meta = previousAttrs.meta // {
        description = "Telegram Desktop based client with optional privacy and customization features";
        longDescription = ''
          SereinGram is an open-source Telegram client based on Telegram Desktop
          and inspired by Nagram and AyuGram. Every addition is off by default.
        '';
        homepage = "https://github.com/eltavine/SereinGram";
        changelog = "https://github.com/eltavine/SereinGram/releases";
        license = lib.licenses.gpl3Only;
        maintainers = [ ];
        mainProgram = "SereinGram";
        platforms = lib.platforms.linux;
      };
    }
  );
in
telegram-desktop.override {
  pname = "sereingram";
  inherit unwrapped;
}
