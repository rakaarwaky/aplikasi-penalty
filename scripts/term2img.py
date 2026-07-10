#!/usr/bin/env python3
"""Terminal ANSI → PNG via aha (ANSI→HTML) + Chromium (HTML→PNG)."""

import sys, os, subprocess, tempfile

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
  line-height: 1.25;
  padding: 12px 16px;
  color: #cccccc;
  white-space: pre;
  display: inline-block;
}}
</style>
</head>
<body>
{body}
</body>
</html>
"""

def ansi_to_png(input_txt, output_png, font_size=16):
    """Convert ANSI text file to PNG using aha + Chromium."""
    # Step 1: ANSI → HTML via aha
    with open(input_txt, "r") as f:
        ansi_text = f.read()

    proc = subprocess.run(
        ["aha", "--no-header"],
        input=ansi_text, capture_output=True, text=True
    )
    if proc.returncode != 0:
        print(f"aha error: {proc.stderr}", file=sys.stderr)
        return False
    body_html = proc.stdout

    # Step 2: Wrap in full HTML
    full_html = HTML_TEMPLATE.format(font_size=font_size, body=body_html)

    # Step 3: Write temp HTML
    with tempfile.NamedTemporaryFile(suffix=".html", delete=False, mode="w") as f:
        f.write(full_html)
        tmp_html = f.name

    # Step 4: HTML → PNG via html2image (Chromium)
    try:
        from html2image import Html2Image
        hti = Html2Image(
            browser_executable="/usr/bin/chromium-browser",
            output_path=os.path.dirname(os.path.abspath(output_png)),
            custom_flags=[
                "--no-sandbox",
                "--disable-gpu",
                "--disable-software-rasterizer",
                "--hide-scrollbars",
            ]
        )
        base = os.path.splitext(os.path.basename(output_png))[0]
        hti.screenshot(html_file=tmp_html, save_as=f"{base}.png", size=(1200, 800))
        # html2image saves to output_path, may add extra size param
        result = os.path.join(os.path.dirname(os.path.abspath(output_png)), f"{base}.png")
        if not os.path.exists(result):
            # Fallback: try with different name
            hti.screenshot(html_str=full_html, save_as=f"{base}.png", size=(1200, 800))
    finally:
        os.unlink(tmp_html)

    if os.path.exists(output_png):
        sz = os.path.getsize(output_png)
        print(f"OK: {output_png} ({sz} bytes)")
        return True
    else:
        print(f"FAIL: {output_png} not created", file=sys.stderr)
        return False

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <input.txt> <output.png> [font_size]")
        sys.exit(1)
    fs = int(sys.argv[3]) if len(sys.argv) > 3 else 16
    ok = ansi_to_png(sys.argv[1], sys.argv[2], fs)
    sys.exit(0 if ok else 1)
