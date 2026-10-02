# SereinGram branding

SereinGram is an independent Telegram client based on Telegram Desktop.
Upstream source, history, license and copyright notices remain with their
respective authors. SereinGram is not affiliated with Telegram, Nagram or
AyuGram.

Application identity:

- Display name, executable and macOS bundle: SereinGram.
- macOS and Linux application ID: `io.github.eltavine.SereinGram`.
- Windows application ID: `eltavine.SereinGram`, with its own installer ID and
  notification activator.
- Data directory: `SereinGram` (on macOS `Application Support/SereinGram`);
  portable directory: `SereinGramForcePortable`. Existing Telegram or Nagram
  profiles are not imported.
- Telegram `tg:` and `tonsite:` protocol compatibility is retained.

Icons:

- The SereinGram logo was designed by
  [OukaroMF](https://github.com/OukaroMF/). Its master file,
  `Telegram/Resources/branding/sereingram-logo.svg`, is kept exactly as
  delivered.
- `tools/serein/brand/generate_icons.py` renders the logo with resvg
  (`uv run --with pillow==12.3.0 --with resvg-py==0.5.0 python
  tools/serein/brand/generate_icons.py`). Every application icon places it on
  a light rounded tile that follows the macOS icon grid: the PNG sizes, the
  Windows ICO, the macOS icon sets, the round and green variants and the
  master `Telegram/Resources/branding/sereingram.png`.
- The monochrome tray glyphs, the settings entry glyph, the QR login glyph and
  the symbolic icon reuse the dark paths of the logo and leave its lighter
  folds open.
- No Nagram or Telegram artwork is used for the application identity.

Upstream automatic updates and crash submission are disabled at build time.
Store distribution is not configured.
