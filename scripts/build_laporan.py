#!/usr/bin/env python3
"""Build Laporan-Project.pdf from Laporan-Project.md.

Pipeline:
  .md -> render ```mermaid``` blocks to crisp PNG (SVG@mermaid.ink -> magick 300dpi)
      -> wrap in HTML -> chromium headless --print-to-pdf
"""
import base64, re, os, subprocess, tempfile, shutil

SRC = 'Laporan-Project.md'
OUT = 'Laporan-Project.pdf'
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)  # project root (scripts/ is one level down)
os.chdir(ROOT)

THEME_JSON = os.path.join(ROOT, 'build_assets', 'mermaid_theme.json')

def render_mermaid(src: str, out_png: str):
    # Render lokal pakai mermaid-cli (mmdc) via npx — text PASTI ada (bukan mermaid.ink
    # yang drop text node). Theme hitam-putih di build_assets/mermaid_theme.json.
    # scale 3 -> resolusi tajam, font 24px -> text jelas di PDF.
    mmd = os.path.join(tempfile.gettempdir(), 'mmd_in.mmd')
    with open(mmd, 'w', encoding='utf-8') as f:
        f.write(src)
    subprocess.run(['npx', '--yes', '@mermaid-js/mermaid-cli',
                    '-i', mmd, '-o', out_png,
                    '-t', 'neutral', '-C', THEME_JSON, '-s', '3'],
                   check=True, timeout=120)

def md_to_html(md: str, imgdir: str) -> str:
    # Extract mermaid blocks, replace with <img> referencing rendered PNG.
    lines = md.split('\n')
    out, i, idx = [], 0, 0
    while i < len(lines):
        if lines[i].strip() == '```mermaid':
            buf = []
            i += 1
            while i < len(lines) and lines[i].strip() != '```':
                buf.append(lines[i]); i += 1
            i += 1  # skip closing ```
            png = os.path.join(imgdir, f'mermaid_{idx}.png')
            render_mermaid('\n'.join(buf), png)
            rel = os.path.relpath(png, ROOT)
            out.append(f'<div class="fig"><img src="{rel}" style="max-width:100%"/></div>')
            idx += 1
        else:
            out.append(lines[i]); i += 1
    body = '\n'.join(out)
    # Minimal markdown -> html: headings, code, lists, tables.
    import html
    body = html.escape(body)  # safe; we re-add tags below for known constructs
    return body  # placeholder; real converter below


def simple_markdown(md: str) -> str:
    """Tiny markdown subset sufficient for this report."""
    lines = md.split('\n')
    html_lines = []
    i = 0
    in_code = False
    code_buf = []
    table_buf = []
    while i < len(lines):
        line = lines[i]
        if line.strip().startswith('```mermaid'):
            html_lines.append('__MERMAID__')
            i += 1
            while i < len(lines) and not lines[i].strip().startswith('```'):
                i += 1
            i += 1
            continue
        if line.strip().startswith('```'):
            if not in_code:
                in_code = True; code_buf = []; i += 1; continue
            else:
                in_code = False
                code = '\n'.join(code_buf)
                html_lines.append('<pre><code>' + _esc(code) + '</code></pre>')
                i += 1; continue
        if in_code:
            code_buf.append(line); i += 1; continue
        # Table handling: collect a | row; optionally a |---| separator
        s = line.rstrip()
        if s.strip().startswith('|') and '|' in s[1:]:
            rows = [s]
            i += 1
            has_sep = (i < len(lines) and re.match(r'^\s*\|?[\s:|-]+\|?\s*$', lines[i]) and '-' in lines[i])
            if has_sep:
                i += 1  # consume separator
            while i < len(lines) and lines[i].strip().startswith('|') and '|' in lines[i].strip()[1:]:
                rows.append(lines[i]); i += 1
            def cells(r):
                return [c.strip() for c in r.strip().strip('|').split('|')]
            if has_sep:
                thead = '<tr>' + ''.join(f'<th>{_esc(c)}</th>' for c in cells(rows[0])) + '</tr>'
                data = rows[1:]
                tbl = f'<table><thead>{thead}</thead><tbody>'
            else:
                tbl = '<table><tbody>'
                data = rows
            for r in data:
                tbl += '<tr>' + ''.join(f'<td>{_esc(c)}</td>' for c in cells(r)) + '</tr>'
            tbl += '</tbody></table>'
            html_lines.append(tbl)
            continue
        if s.startswith('# '):
            html_lines.append(f'<h1>{_esc(s[2:])}</h1>')
        elif s.startswith('## '):
            html_lines.append(f'<h2>{_esc(s[3:])}</h2>')
        elif s.startswith('### '):
            html_lines.append(f'<h3>{_esc(s[4:])}</h3>')
        elif s.startswith('- '):
            html_lines.append(f'<li>{_esc(s[2:])}</li>')
        elif s.strip() == '---':
            html_lines.append('<hr/>')
        elif s.strip() == '':
            html_lines.append('')
        else:
            html_lines.append(f'<p>{_esc(s)}</p>')
        i += 1
    # wrap consecutive <li> in <ul>
    wrapped = []
    buf = []
    for ln in html_lines:
        if ln.startswith('<li>'):
            buf.append(ln)
        else:
            if buf:
                wrapped.append('<ul>' + ''.join(buf) + '</ul>'); buf = []
            wrapped.append(ln)
    if buf:
        wrapped.append('<ul>' + ''.join(buf) + '</ul>')
    return '\n'.join(wrapped)


def _esc(t: str) -> str:
    import html
    return html.escape(t)


def build():
    md = open(SRC, encoding='utf-8').read()
    # Split mermaid blocks out, render to PNG, replace with markers.
    parts = re.split(r'(```mermaid\n.*?```)', md, flags=re.DOTALL)
    imgdir = os.path.join(ROOT, 'build_assets')
    os.makedirs(imgdir, exist_ok=True)
    md_with_markers = []
    mermaid_srcs = []
    for p in parts:
        if p.startswith('```mermaid'):
            src = p[len('```mermaid'):].strip().removesuffix('```').strip()
            mermaid_srcs.append(src)
            md_with_markers.append('__MERMAID__')
        else:
            md_with_markers.append(p)
    md2 = ''.join(md_with_markers)

    body = simple_markdown(md2)
    # inject images in order
    imgs_html = []
    for idx, src in enumerate(mermaid_srcs):
        png = os.path.join(imgdir, f'mermaid_{idx}.png')
        render_mermaid(src, png)
        imgs_html.append(f'<div class="fig"><img src="file://{png}"/></div>')
    for img in imgs_html:
        body = body.replace('__MERMAID__', img, 1)

    html_doc = f"""<!doctype html><html><head><meta charset="utf-8">
<style>
  body {{ font-family: Calibri, Arial, sans-serif; font-size: 11pt; margin: 2cm; color:#111; }}
  h1 {{ font-size: 20pt; text-align:center; }}
  h2 {{ font-size: 15pt; border-bottom:1px solid #888; padding-bottom:3px; }}
  h3 {{ font-size: 12.5pt; }}
  pre {{ background:#f5f5f5; padding:10px; border-radius:4px; font-family:Consolas,monospace; font-size:9pt; overflow-x:auto; }}
  code {{ font-family:Consolas,monospace; font-size:9pt; }}
  table {{ border-collapse:collapse; width:60%; margin:10px 0; }}
  th,td {{ border:1px solid #999; padding:5px 8px; text-align:left; }}
  .fig {{ text-align:center; margin:14px 0; page-break-inside: avoid; }}
  .fig img {{ width:100%; height:auto; }}
  hr {{ margin:18px 0; }}
</style></head><body>{body}</body></html>"""

    html_path = os.path.join(ROOT, 'build_assets', 'report.html')
    with open(html_path, 'w', encoding='utf-8') as f:
        f.write(html_doc)

    last = None
    for attempt in range(3):
        try:
            subprocess.run(['chromium-browser', '--headless', '--no-sandbox',
                            '--disable-gpu', '--disable-dev-shm-usage',
                            '--no-pdf-header-footer',
                            f'--print-to-pdf={os.path.join(ROOT, OUT)}', html_path],
                           check=True, timeout=90)
            last = None
            break
        except subprocess.CalledProcessError as e:
            last = e
            print(f'  attempt {attempt+1} failed (SIGTRAP?), retrying...')
    if last:
        raise last
    print(f'SAVED {OUT}')


if __name__ == '__main__':
    build()
