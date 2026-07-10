#!/usr/bin/env python3
"""Terminal ANSI → PNG via aha + Chromium. Bright colors, full black bg, auto-crop."""

import sys, os, subprocess, tempfile, re
from PIL import Image

COLOR_FIX = {
    'color:gray':         'color:#cccccc',
    'color:teal':         'color:#00cccc',
    'color:green':        'color:#00cc00',
    'color:red':          'color:#cc0000',
    'color:blue':         'color:#0000cc',
    'color:cyan':         'color:#00cccc',
    'color:yellow':       'color:#ccaa00',
    'color:magenta':      'color:#cc00cc',
    'color:white':        'color:#ffffff',
    'color:black':        'color:#000000',
    'background-color:gray':   'background-color:#666666',
    'background-color:teal':   'background-color:#00cccc',
    'background-color:green':  'background-color:#00cc00',
    'background-color:red':    'background-color:#cc0000',
    'background-color:blue':   'background-color:#0000cc',
    'background-color:cyan':   'background-color:#00cccc',
    'background-color:yellow': 'background-color:#ccaa00',
    'background-color:magenta':'background-color:#cc00cc',
    'background-color:white':  'background-color:#ffffff',
    'background-color:black':  'background-color:#000000',
    'background-color:navy':   'background-color:#0000cc',
}

HTML_TEMPLATE = """\
<!DOCTYPE html>
<html>
<head><meta charset="utf-8">
<style>
* {{ margin:0; padding:0; }}
body {{
  background: #000000;
  font-family: 'Noto Sans Mono', 'DejaVu Sans Mono', monospace;
  font-size: {font_size}px;
  line-height: 1.15;
  color: #cccccc;
  white-space: pre;
  display: inline-block;
  padding: 4px 8px;
  letter-spacing: 0;
}}
</style>
</head>
<body>{body}</body>
</html>"""

def fix_colors(html):
    for old, new in COLOR_FIX.items():
        html = html.replace(old, new)
    return html

def trim_ansi(text):
    lines = text.split("\n")
    result = []
    in_box = False
    for line in lines:
        clean = re.sub(r'\x1b\[[0-9;]*m', '', line).strip()
        if '╔' in clean or '╠' in clean:
            in_box = True
        if '╚' in clean or '╣' in clean:
            in_box = False
            result.append(line)
            continue
        if clean == "" and not in_box:
            continue
        result.append(line)
    return "\n".join(result)

def auto_crop(png_path):
    img = Image.open(png_path)
    w, h = img.size
    bg = (0, 0, 0)
    bottom = h
    for y in range(h - 1, 0, -1):
        row_bg = all(img.getpixel((x, y)) == bg for x in range(0, w, 4))
        if not row_bg:
            bottom = y + 2
            break
    if bottom < h:
        img = img.crop((0, 0, w, bottom))
        img.save(png_path, "PNG")

def ansi_to_png(input_txt, output_png, font_size=16):
    with open(input_txt) as f:
        raw = f.read()
    raw = trim_ansi(raw)

    proc = subprocess.run(["aha", "--no-header"], input=raw, capture_output=True, text=True)
    if proc.returncode != 0:
        print(f"aha error: {proc.stderr}", file=sys.stderr)
        return False
    body = fix_colors(proc.stdout)
    full = HTML_TEMPLATE.format(font_size=font_size, body=body)

    with tempfile.NamedTemporaryFile(suffix=".html", delete=False, mode="w") as f:
        f.write(full)
        tmp = f.name

    try:
        from html2image import Html2Image
        hti = Html2Image(
            browser_executable="/usr/bin/chromium-browser",
            output_path=os.path.dirname(os.path.abspath(output_png)),
            custom_flags=["--no-sandbox","--disable-gpu","--hide-scrollbars"]
        )
        base = os.path.splitext(os.path.basename(output_png))[0]
        hti.screenshot(html_file=tmp, save_as=f"{base}.png", size=(900, 600))
    finally:
        os.unlink(tmp)

    if os.path.exists(output_png):
        auto_crop(output_png)
        sz = os.path.getsize(output_png)
        print(f"OK: {output_png} ({sz} bytes)")
        return True
    print(f"FAIL: {output_png}", file=sys.stderr)
    return False

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <input.txt> <output.png> [font_size]")
        sys.exit(1)
    fs = int(sys.argv[3]) if len(sys.argv) > 3 else 16
    ok = ansi_to_png(sys.argv[1], sys.argv[2], fs)
    sys.exit(0 if ok else 1)
