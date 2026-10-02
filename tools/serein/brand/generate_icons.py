#!/usr/bin/env python3
"""Generate the SereinGram placeholder icon set.

Run from the repository root:
    uv run --with pillow python tools/serein/brand/generate_icons.py
"""

import math
from pathlib import Path

from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[3]
ART = ROOT / "Telegram/Resources/art"
ICONS = ROOT / "Telegram/Resources/icons"
BRANDING = ROOT / "Telegram/Resources/branding"
XCASSETS = ROOT / "Telegram/Telegram/Images.xcassets"

DUSK = ((27, 42, 92), (122, 111, 240))
GREEN = ((14, 94, 74), (52, 195, 143))
SUPERSAMPLE = 4
GLYPH_SPAN = 0.56

OUTER = ((10.5, 11.0), 7.5)
INNER = ((14.5, 7.5), 6.4)
STREAK_WIDTH = 0.85
STREAKS = (
    ((15.2, 3.4), (14.5, 5.6)),
    ((18.6, 4.4), (17.9, 6.6)),
    ((16.2, 8.9), (15.5, 11.1)),
)


def arc(center, radius, start, end, steps):
    (cx, cy) = center
    return [
        (
            cx + radius * math.cos(start + (end - start) * i / steps),
            cy + radius * math.sin(start + (end - start) * i / steps),
        )
        for i in range(steps + 1)
    ]


def crescent():
    ((ox, oy), big), ((ix, iy), small) = OUTER, INNER
    dx, dy = ix - ox, iy - oy
    distance = math.hypot(dx, dy)
    base = math.atan2(dy, dx)
    outer = math.acos((big * big + distance * distance - small * small) / (2 * big * distance))
    inner = math.acos((big * big - distance * distance - small * small) / (2 * distance * small))
    points = arc(OUTER[0], big, base + outer, base + 2 * math.pi - outer, 120)
    points += arc(INNER[0], small, base - inner, base - 2 * math.pi + inner, 80)[1:-1]
    return points


def capsule(start, end, width):
    (ax, ay), (bx, by) = start, end
    angle = math.atan2(by - ay, bx - ax)
    points = arc(end, width, angle - math.pi / 2, angle + math.pi / 2, 16)
    points += arc(start, width, angle + math.pi / 2, angle + 3 * math.pi / 2, 16)
    return points


def glyph_polygons():
    polygons = [crescent()] + [capsule(a, b, STREAK_WIDTH) for a, b in STREAKS]
    xs = [x for polygon in polygons for x, _ in polygon]
    ys = [y for polygon in polygons for _, y in polygon]
    shift = (12 - (min(xs) + max(xs)) / 2, 12 - (min(ys) + max(ys)) / 2)
    span = max(max(xs) - min(xs), max(ys) - min(ys))
    moved = [[(x + shift[0], y + shift[1]) for x, y in polygon] for polygon in polygons]
    return moved, span


POLYGONS, SPAN = glyph_polygons()


def glyph_mask(size, scale, origin):
    mask = Image.new("L", (size, size), 0)
    draw = ImageDraw.Draw(mask)
    for polygon in POLYGONS:
        draw.polygon([(origin[0] + x * scale, origin[1] + y * scale) for x, y in polygon], fill=255)
    return mask


def gradient(size, colors):
    top, bottom = colors
    column = Image.new("RGB", (1, size))
    for y in range(size):
        t = y / max(size - 1, 1)
        column.putpixel(
            (0, y), tuple(round(a + (b - a) * t) for a, b in zip(top, bottom, strict=True))
        )
    return column.resize((size, size))


def render(size, colors, shape, margin):
    big = size * SUPERSAMPLE
    inset = round(big * margin)
    body = Image.new("L", (big, big), 0)
    draw = ImageDraw.Draw(body)
    box = (inset, inset, big - inset - 1, big - inset - 1)
    if shape == "round":
        draw.ellipse(box, fill=255)
    else:
        draw.rounded_rectangle(box, radius=round((big - 2 * inset) * 0.2245), fill=255)
    image = Image.new("RGBA", (big, big), (0, 0, 0, 0))
    image.paste(gradient(big, colors), (0, 0), body)
    scale = (big - 2 * inset) * GLYPH_SPAN / SPAN
    offset = big / 2 - 12 * scale
    white = Image.new("RGBA", (big, big), (255, 255, 255, 255))
    image.paste(white, (0, 0), glyph_mask(big, scale, (offset, offset)))
    return image.resize((size, size), Image.LANCZOS)


def render_glyph(size):
    big = size * SUPERSAMPLE
    scale = big / 24
    mask = glyph_mask(big, scale, (0, 0))
    image = Image.new("RGBA", (big, big), (255, 255, 255, 0))
    image.putalpha(mask)
    return image.resize((size, size), Image.LANCZOS)


def svg(badge=None, size=24):
    paths = "".join(
        '<path fill="white" d="M' + " L".join(f"{x:.3f} {y:.3f}" for x, y in polygon) + 'Z"/>'
        for polygon in POLYGONS
    )
    circle = f'<circle cx="20" cy="20" r="3.6" fill="{badge}"/>' if badge else ""
    return (
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{size}" height="{size}" '
        f'viewBox="0 0 24 24">{paths}{circle}</svg>\n'
    )


def main():
    BRANDING.mkdir(parents=True, exist_ok=True)
    squares = {
        size: render(size, DUSK, "square", 0.0977)
        for size in (16, 32, 48, 64, 96, 128, 256, 512, 1024)
    }
    for size in (16, 32, 48, 64, 128, 256, 512):
        squares[size].save(ART / f"icon{size}.png")
        squares[size * 2].save(ART / f"icon{size}@2x.png")
    squares[256].save(ART / "icon256.ico", sizes=[(s, s) for s in (16, 24, 32, 48, 64, 128, 256)])
    render(1024, GREEN, "square", 0.0977).save(ART / "icon_green.png")
    render(256, GREEN, "square", 0.0977).save(ART / "iconbig_green.png")
    render(1024, DUSK, "round", 0.0).save(ART / "icon_round512@2x.png")
    render(256, DUSK, "round", 0.0625).save(ART / "logo_256.png")
    render(256, DUSK, "round", 0.0).save(ART / "logo_256_no_margin.png")
    squares[1024].save(BRANDING / "sereingram.png")
    for scale, suffix in ((1, ""), (2, "@2x"), (3, "@3x")):
        render_glyph(35 * scale).save(ICONS / f"intro_qr_plane{suffix}.png")
    for size in (16, 32, 128, 256, 512):
        squares[size].save(XCASSETS / f"Icon.appiconset/icon{size}.png")
        squares[size * 2].save(XCASSETS / f"Icon.appiconset/icon{size}@2x.png")
        squares[size].save(XCASSETS / f"Icon.iconset/icon_{size}x{size}.png")
        squares[size * 2].save(XCASSETS / f"Icon.iconset/icon_{size}x{size}@2x.png")
    (ICONS / "tray_monochrome.svg").write_text(svg(size=16), encoding="utf-8")
    (ICONS / "tray_monochrome_attention.svg").write_text(svg("#ff4b4b", 16), encoding="utf-8")
    (ICONS / "tray_monochrome_mute.svg").write_text(svg("#888888", 16), encoding="utf-8")
    (ICONS / "menu/serein.svg").write_text(svg(), encoding="utf-8")
    (BRANDING / "sereingram-symbolic.svg").write_text(svg(size=16), encoding="utf-8")


if __name__ == "__main__":
    main()
