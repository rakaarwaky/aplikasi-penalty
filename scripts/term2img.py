#!/usr/bin/env python3
"""Render ANSI terminal output to PNG using pyte (pixel-perfect terminal emulator)."""

import sys, os
import pyte
from PIL import Image, ImageDraw, ImageFont

# Standard ANSI colors (xterm-256 base)
COLORS = [
    (0,0,0),         # 0 black
    (170,0,0),       # 1 red
    (0,170,0),       # 2 green
    (170,85,0),      # 3 yellow
    (0,0,170),       # 4 blue
    (170,0,170),     # 5 magenta
    (0,170,170),     # 6 cyan
    (170,170,170),   # 7 white
    (85,85,85),      # 8 bright black
    (255,85,85),     # 9 bright red
    (85,255,85),     # 10 bright green
    (255,255,85),    # 11 bright yellow
    (85,85,255),     # 12 bright blue
    (255,85,255),    # 13 bright magenta
    (85,255,255),    # 14 bright cyan
    (255,255,255),   # 15 bright white
]

def find_font(size):
    for p in [
        "/usr/share/fonts/dejavu-sans-mono-fonts/DejaVuSansMono.ttf",
        "/usr/share/fonts/liberation-mono-fonts/LiberationMono-Regular.ttf",
    ]:
        if os.path.exists(p):
            return ImageFont.truetype(p, size)
    return ImageFont.load_default()

def ansi_to_png(input_txt, output_png, font_size=16):
    with open(input_txt, "r") as f:
        raw = f.read()

    # Strip trailing newlines
    raw = raw.rstrip("\n")

    # Use pyte to parse ANSI and build character grid
    cols = 80
    rows = raw.count("\n") + 1
    screen = pyte.Screen(cols, rows)
    stream = pyte.Stream(screen)
    stream.feed(raw)

    # Font metrics
    font = find_font(font_size)
    tmp = ImageDraw.Draw(Image.new("RGB", (1,1)))
    bb = tmp.textbbox((0,0), "M", font=font)
    cw = bb[2] - bb[0]
    ch = bb[3] - bb[1]

    padding = 8
    img_w = cols * cw + padding * 2
    img_h = rows * ch + padding * 2
    BG_DEFAULT = (30, 30, 30)

    img = Image.new("RGB", (img_w, img_h), BG_DEFAULT)
    draw = ImageDraw.Draw(img)

    for y in range(rows):
        for x in range(cols):
            cell = screen.buffer[y][x]
            char = cell.data if cell.data else " "

            # Resolve colors
            fg_idx = cell.fg if cell.fg else "default"
            bg_idx = cell.bg if cell.bg else "default"

            if fg_idx == "default":
                fg = (170, 170, 170)
            else:
                try:
                    fg = COLORS[int(fg_idx)]
                except (ValueError, IndexError):
                    fg = (170, 170, 170)

            if bg_idx == "default":
                bg = BG_DEFAULT
            else:
                try:
                    bg = COLORS[int(bg_idx)]
                except (ValueError, IndexError):
                    bg = BG_DEFAULT

            # Bold → brighter
            if cell.bold:
                fg = tuple(min(255, c + 85) for c in fg)

            px = padding + x * cw
            py_pos = padding + y * ch

            # Draw background
            if bg != BG_DEFAULT:
                draw.rectangle([px, py_pos, px + cw - 1, py_pos + ch - 1], fill=bg)

            # Draw character
            if char != " ":
                draw.text((px, py_pos), char, fill=fg, font=font)

    img.save(output_png, "PNG")
    sz = os.path.getsize(output_png)
    print(f"OK: {output_png} ({sz} bytes, {img_w}x{img_h})")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <input.txt> <output.png> [font_size]")
        sys.exit(1)
    fs = int(sys.argv[3]) if len(sys.argv) > 3 else 16
    ansi_to_png(sys.argv[1], sys.argv[2], fs)
