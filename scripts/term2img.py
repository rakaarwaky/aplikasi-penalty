#!/usr/bin/env python3
"""Terminal ANSI → PNG via aha (ANSI→HTML) + Chromium (HTML→PNG).
Post-processes aha output to fix color mapping."""

import sys, os, subprocess, tempfile, re

# ── Fix aha's color names to match actual ncurses terminal colors ──
COLOR_FIX = {
    # aha maps \033[37m to "gray" — we want the actual terminal white
    'color:gray':         'color:#cccccc',
    'color:teal':         'color:#00aaaa',
    'color:green':        'color:#00aa00',
    'color:red':          'color:#aa0000',
    'color:blue':         'color:#0000aa',
    'color:cyan':         'color:#00aaaa',
    'color:yellow':       'color:#aa5500',
    'color:magenta':      'color:#aa00aa',
    'color:white':        'color:#ffffff',
    'color:black':        'color:#000000',
    'background-color:gray':   'background-color:#555555',
    'background-color:teal':   'background-color:#00aaaa',
    'background-color:green':  'background-color:#00aa00',
    'background-color:red':    'background-color:#aa0000',
    'background-color:blue':   'background-color:#0000aa',
    'background-color:cyan':   'background-color:#00aaaa',
    'background-color:yellow': 'background-color:#aa5500',
    'background-color:magenta':'background-color:#aa00aa',
    'background-color:white':  'background-color:#ffffff',
    'background-color:black':  'background-color:#000000',
    'background-color:navy':   'background-color:#0000aa',
}

HTML_TEMPLATE = """\
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<style>
* {{ margin:0; padding:0; box-sizing:border-box; }}
body {{
  background: #1e1e1e;
  font-family: 'DejaVu Sans Mono', 'Liberation Mono', 'Courier New', monospace;
  font-size: {font_size}px;
  line-height: 1.3;
  color: #cccccc;
  white-space: pre;
}}
.wrap {{
  display: inline-block;
  padding: 8px 12px;
}}
</style>
</head>
<body>
<div class="wrap">
{body}
</div>
</body>
</html>
"""

def fix_aha_colors(html):
    """Replace aha's inaccurate color names with hex values."""
    for old, new in COLOR_FIX.items():
        html = html.replace(old, new)
    return html

def ansi_to_png(input_txt, output_png, font_size=16):
    with open(input_txt, "r") as f:
        ansi_text = f.read()

    proc = subprocess.run(
        ["aha", "--no-header"],
        input=ansi_text, capture_output=True, text=True
    )
    if proc.returncode != 0:
        print(f"aha error: {proc.stderr}", file=sys.stderr)
        return False
    body_html = fix_aha_colors(proc.stdout)
    full_html = HTML_TEMPLATE.format(font_size=font_size, body=body_html)

    with tempfile.NamedTemporaryFile(suffix=".html", delete=False, mode="w") as f:
        f.write(full_html)
        tmp_html = f.name

    try:
        from html2image import Html2Image
        hti = Html2Image(
            browser_executable="/usr/bin/chromium-browser",
            output_path=os.path.dirname(os.path.abspath(output_png)),
            custom_flags=[
                "--no-sandbox", "--disable-gpu",
                "--disable-software-rasterizer", "--hide-scrollbars",
            ]
        )
        base = os.path.splitext(os.path.basename(output_png))[0]
        hti.screenshot(html_file=tmp_html, save_as=f"{base}.png", size=(900, 600))
        result = os.path.join(os.path.dirname(os.path.abspath(output_png)), f"{base}.png")
        if not os.path.exists(result):
            hti.screenshot(html_str=full_html, save_as=f"{base}.png", size=(900, 600))
    finally:
        os.unlink(tmp_html)

    if os.path.exists(output_png):
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
