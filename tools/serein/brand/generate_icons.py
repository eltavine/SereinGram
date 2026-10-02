#!/usr/bin/env python3
"""Generate the SereinGram icon set from the master logo.

The logo was designed by OukaroMF (https://github.com/OukaroMF/). Its master
file, Telegram/Resources/branding/sereingram-logo.svg, is kept exactly as
delivered; every application icon places it on a light tile, and the
monochrome glyphs reuse its dark paths, leaving the lighter folds open.

Run from the repository root:
    uv run --with pillow==12.3.0 --with resvg-py==0.5.0 \\
        python tools/serein/brand/generate_icons.py
"""

import io
import xml.etree.ElementTree as ET
from pathlib import Path

import resvg_py
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[3]
ART = ROOT / "Telegram/Resources/art"
ICONS = ROOT / "Telegram/Resources/icons"
BRANDING = ROOT / "Telegram/Resources/branding"
XCASSETS = ROOT / "Telegram/Telegram/Images.xcassets"
MASTER = BRANDING / "sereingram-logo.svg"

SVG_NAMESPACE = "http://www.w3.org/2000/svg"
LIGHT = ((255, 255, 255), (244, 244, 245), (228, 228, 231))
GREEN = ((240, 253, 244), (220, 252, 231), (187, 247, 208))
DARK_FILL = "#09090B"
SUPERSAMPLE = 4
GLYPH_SPAN = 0.74
BORDER = 0.008
SQUARE_MARGIN = 0.0977
SQUARE_RADIUS = 0.2245

ET.register_namespace("", SVG_NAMESPACE)


def master():
    return ET.fromstring(MASTER.read_text(encoding="utf-8"))


def render_svg(root, size):
    text = ET.tostring(root, encoding="unicode")
    data = resvg_py.svg_to_bytes(svg_string=text, width=size, height=size)
    return Image.open(io.BytesIO(bytes(data))).convert("RGBA")


def glyph_box():
    size = 2048
    root = master()
    bbox = render_svg(root, size).getchannel("A").getbbox()
    scale = float(root.get("viewBox").split()[2]) / size
    return tuple(value * scale for value in bbox)


BOX = glyph_box()


def framed(root, size, span):
    x0, y0, x1, y1 = BOX
    side = max(x1 - x0, y1 - y0) / span
    center = ((x0 + x1) / 2, (y0 + y1) / 2)
    root.set(
        "viewBox", f"{center[0] - side / 2:.4f} {center[1] - side / 2:.4f} {side:.4f} {side:.4f}"
    )
    root.set("width", str(size))
    root.set("height", str(size))
    return root


def glyph(size, span):
    return render_svg(framed(master(), size, span), size)


def tile(size, colors, shape, margin):
    big = size * SUPERSAMPLE
    inset = round(big * margin)
    box = (inset, inset, big - inset - 1, big - inset - 1)
    side = big - 2 * inset
    top, bottom, border = colors
    column = Image.new("RGBA", (1, big))
    for y in range(big):
        t = y / max(big - 1, 1)
        rgb = tuple(round(a + (b - a) * t) for a, b in zip(top, bottom, strict=True))
        column.putpixel((0, y), (*rgb, 255))
    fill = column.resize((big, big))
    mask = Image.new("L", (big, big), 0)
    outline = Image.new("RGBA", (big, big), (0, 0, 0, 0))
    width = max(1, round(side * BORDER))
    if shape == "round":
        ImageDraw.Draw(mask).ellipse(box, fill=255)
        ImageDraw.Draw(outline).ellipse(box, outline=(*border, 255), width=width)
    else:
        radius = round(side * SQUARE_RADIUS)
        ImageDraw.Draw(mask).rounded_rectangle(box, radius=radius, fill=255)
        ImageDraw.Draw(outline).rounded_rectangle(
            box, radius=radius, outline=(*border, 255), width=width
        )
    image = Image.new("RGBA", (big, big), (0, 0, 0, 0))
    image.paste(fill, (0, 0), mask)
    image.alpha_composite(outline)
    return image.resize((size, size), Image.LANCZOS)


def render(size, colors=LIGHT, shape="square", margin=SQUARE_MARGIN):
    image = tile(size, colors, shape, margin)
    inner = 1 - 2 * margin
    image.alpha_composite(glyph(size, GLYPH_SPAN * inner))
    return image


def monochrome(size, padding, badge=None):
    root = master()
    root.text = None
    for element in list(root):
        element.tail = None
        if element.get("fill", "").upper() == DARK_FILL:
            element.set("fill", "white")
        else:
            root.remove(element)
    framed(root, size, 1 - 2 * padding)
    if badge:
        x, y, side, _ = (float(value) for value in root.get("viewBox").split())
        ET.SubElement(
            root,
            f"{{{SVG_NAMESPACE}}}circle",
            cx=f"{x + side * 0.8333:.4f}",
            cy=f"{y + side * 0.8333:.4f}",
            r=f"{side * 0.15:.4f}",
            fill=badge,
        )
    return root


def monochrome_svg(size, padding, badge=None):
    text = ET.tostring(monochrome(size, padding, badge), encoding="unicode")
    return text + "\n"


def main():
    squares = {size: render(size) for size in (16, 32, 48, 64, 96, 128, 256, 512, 1024)}
    for size in (16, 32, 48, 64, 128, 256, 512):
        squares[size].save(ART / f"icon{size}.png")
        squares[size * 2].save(ART / f"icon{size}@2x.png")
    squares[256].save(ART / "icon256.ico", sizes=[(s, s) for s in (16, 24, 32, 48, 64, 128, 256)])
    render(1024, GREEN).save(ART / "icon_green.png")
    render(256, GREEN).save(ART / "iconbig_green.png")
    render(1024, shape="round", margin=0.0).save(ART / "icon_round512@2x.png")
    render(256, shape="round", margin=0.0625).save(ART / "logo_256.png")
    render(256, shape="round", margin=0.0).save(ART / "logo_256_no_margin.png")
    squares[1024].save(BRANDING / "sereingram.png")
    for scale, suffix in ((1, ""), (2, "@2x"), (3, "@3x")):
        size = 35 * scale
        render_svg(monochrome(size, 0.0), size).save(ICONS / f"intro_qr_plane{suffix}.png")
    for size in (16, 32, 128, 256, 512):
        squares[size].save(XCASSETS / f"Icon.appiconset/icon{size}.png")
        squares[size * 2].save(XCASSETS / f"Icon.appiconset/icon{size}@2x.png")
        squares[size].save(XCASSETS / f"Icon.iconset/icon_{size}x{size}.png")
        squares[size * 2].save(XCASSETS / f"Icon.iconset/icon_{size}x{size}@2x.png")
    tray = {
        "tray_monochrome.svg": None,
        "tray_monochrome_attention.svg": "#ff4b4b",
        "tray_monochrome_mute.svg": "#888888",
    }
    for name, badge in tray.items():
        (ICONS / name).write_text(monochrome_svg(16, 0.04, badge), encoding="utf-8")
    (ICONS / "menu/serein.svg").write_text(monochrome_svg(24, 0.1), encoding="utf-8")
    (BRANDING / "sereingram-symbolic.svg").write_text(monochrome_svg(16, 0.04), encoding="utf-8")


if __name__ == "__main__":
    main()
